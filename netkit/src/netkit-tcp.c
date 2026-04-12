/** Copyright (c) 2026, abc <xxx@gmail.com>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

#include "netkit/netkit-tcp.h"
#include "netkit/netkit-err.h"

#include "tcp.h"
#include "loop.h"
#include "err.h"

#include <mswsock.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <string.h>

/* Load ConnectEx function pointer via WSAIoctl. */
static LPFN_CONNECTEX _tcp_get_connectex(SOCKET sock) {
    LPFN_CONNECTEX fn = NULL;
    GUID guid = WSAID_CONNECTEX;
    DWORD bytes = 0;
    WSAIoctl(
        sock,
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &guid,
        sizeof(guid),
        &fn,
        sizeof(fn),
        &bytes,
        NULL,
        NULL);
    return fn;
}

/* Load AcceptEx function pointer via WSAIoctl. */
static LPFN_ACCEPTEX _tcp_get_acceptex(SOCKET sock) {
    LPFN_ACCEPTEX fn = NULL;
    GUID guid = WSAID_ACCEPTEX;
    DWORD bytes = 0;
    WSAIoctl(
        sock,
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &guid,
        sizeof(guid),
        &fn,
        sizeof(fn),
        &bytes,
        NULL,
        NULL);
    return fn;
}

/* Post a new AcceptEx operation for the server socket. */
static int _tcp_post_accept(netkit_tcp_t* tcp) {
    _op_t* op = _op_create(
        _OP_ACCEPT, tcp, (void*)tcp->accept_cb, tcp->accept_data);
    if (!op) {
        return -1;
    }

    op->accept_sock = WSASocketW(
        AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (op->accept_sock == INVALID_SOCKET) {
        _op_destroy(op);
        return -1;
    }

    LPFN_ACCEPTEX accept_ex = _tcp_get_acceptex(tcp->sock);
    if (!accept_ex) {
        _op_destroy(op);
        return -1;
    }

    DWORD bytes = 0;
    BOOL ok = accept_ex(
        tcp->sock,
        op->accept_sock,
        op->addr_buf,
        0,
        sizeof(struct sockaddr_in6) + 16,
        sizeof(struct sockaddr_in6) + 16,
        &bytes,
        &op->overlapped);

    if (!ok && WSAGetLastError() != ERROR_IO_PENDING) {
        _op_destroy(op);
        return -1;
    }

    tcp->base.pending_ops++;
    return 0;
}

netkit_tcp_t* netkit_tcp_create(netkit_loop_t* loop) {
    if (!loop) {
        return NULL;
    }

    SOCKET sock = WSASocketW(
        AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (sock == INVALID_SOCKET) {
        return NULL;
    }

    netkit_tcp_t* tcp = (netkit_tcp_t*)calloc(1, sizeof(netkit_tcp_t));
    if (!tcp) {
        closesocket(sock);
        return NULL;
    }

    tcp->base.type = _HANDLE_TCP;
    tcp->base.loop = loop;
    tcp->sock      = sock;

    if (_loop_associate(loop, sock) != 0) {
        closesocket(sock);
        free(tcp);
        return NULL;
    }

    _loop_handle_register(loop);
    return tcp;
}

int netkit_tcp_bind(netkit_tcp_t* tcp, const char* ip, int32_t port) {
    if (!tcp || !ip || tcp->base.closing) {
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons((uint16_t)port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1) {
        return -1;
    }

    if (bind(tcp->sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        return -1;
    }
    return 0;
}

int netkit_tcp_listen(
    netkit_tcp_t* tcp,
    int32_t backlog,
    netkit_tcp_accept_fn_t cb,
    void* data) {

    if (!tcp || !cb || tcp->base.closing) {
        return -1;
    }

    if (listen(tcp->sock, backlog) == SOCKET_ERROR) {
        return -1;
    }

    tcp->accept_cb   = cb;
    tcp->accept_data = data;

    return _tcp_post_accept(tcp);
}

int netkit_tcp_connect(
    netkit_tcp_t* tcp,
    const char* ip,
    int32_t port,
    netkit_tcp_connect_fn_t cb,
    void* data) {

    if (!tcp || !ip || !cb || tcp->base.closing) {
        return -1;
    }

    /* ConnectEx requires the socket to be bound first. */
    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family      = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port        = 0;
    if (bind(tcp->sock, (struct sockaddr*)&local, sizeof(local))
        == SOCKET_ERROR) {
        return -1;
    }

    struct sockaddr_in remote;
    memset(&remote, 0, sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_port   = htons((uint16_t)port);
    if (inet_pton(AF_INET, ip, &remote.sin_addr) != 1) {
        return -1;
    }

    LPFN_CONNECTEX connect_ex = _tcp_get_connectex(tcp->sock);
    if (!connect_ex) {
        return -1;
    }

    _op_t* op = _op_create(_OP_CONNECT, tcp, (void*)cb, data);
    if (!op) {
        return -1;
    }

    BOOL ok = connect_ex(
        tcp->sock,
        (struct sockaddr*)&remote,
        sizeof(remote),
        NULL,
        0,
        NULL,
        &op->overlapped);

    if (!ok && WSAGetLastError() != ERROR_IO_PENDING) {
        _op_destroy(op);
        return -1;
    }

    tcp->base.pending_ops++;
    return 0;
}

int netkit_tcp_read(
    netkit_tcp_t* tcp,
    netkit_tcp_read_fn_t cb,
    void* data) {

    if (!tcp || !cb || tcp->base.closing) {
        return -1;
    }

    _op_t* op = _op_create(_OP_READ, tcp, (void*)cb, data);
    if (!op) {
        return -1;
    }

    op->wsa_buf.buf = (char*)op->read_buf;
    op->wsa_buf.len = OP_READ_BUF_SIZE;

    DWORD flags = 0;
    int rc = WSARecv(
        tcp->sock,
        &op->wsa_buf,
        1,
        NULL,
        &flags,
        &op->overlapped,
        NULL);

    if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        _op_destroy(op);
        return -1;
    }

    tcp->base.pending_ops++;
    return 0;
}

int netkit_tcp_write(
    netkit_tcp_t* tcp,
    const void* buf,
    int32_t len,
    netkit_tcp_write_fn_t cb,
    void* data) {

    if (!tcp || !buf || len <= 0 || tcp->base.closing) {
        return -1;
    }

    _op_t* op = _op_create(_OP_WRITE, tcp, (void*)cb, data);
    if (!op) {
        return -1;
    }

    op->wsa_buf.buf = (char*)buf;
    op->wsa_buf.len = (ULONG)len;

    int rc = WSASend(
        tcp->sock,
        &op->wsa_buf,
        1,
        NULL,
        0,
        &op->overlapped,
        NULL);

    if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        _op_destroy(op);
        return -1;
    }

    tcp->base.pending_ops++;
    return 0;
}

void netkit_tcp_close(
    netkit_tcp_t* tcp,
    netkit_close_fn_t cb,
    void* data) {

    if (!tcp || tcp->base.closing) {
        return;
    }

    tcp->base.closing    = true;
    tcp->base.close_cb   = cb;
    tcp->base.close_data = data;

    /* Cancel all pending I/O on this socket. */
    if (tcp->sock != INVALID_SOCKET) {
        CancelIoEx((HANDLE)tcp->sock, NULL);
        closesocket(tcp->sock);
        tcp->sock = INVALID_SOCKET;
    }

    /*
     * If no pending ops and not inside a dispatch handler, finalize now.
     * If inside a dispatch handler, the dispatch function will finalize
     * after the handler returns.
     */
    if (tcp->base.pending_ops == 0 && !tcp->base.in_dispatch) {
        if (cb) {
            cb(tcp, data);
        }
        _loop_handle_unregister(tcp->base.loop);
        free(tcp);
    }
}

void _tcp_on_accept(_op_t* op, DWORD bytes, BOOL success) {
    (void)bytes;
    netkit_tcp_t* server = (netkit_tcp_t*)op->handle;
    netkit_tcp_accept_fn_t cb = (netkit_tcp_accept_fn_t)op->cb;

    if (!success || server->base.closing) {
        if (op->accept_sock != INVALID_SOCKET) {
            closesocket(op->accept_sock);
            op->accept_sock = INVALID_SOCKET;
        }
        if (cb && !server->base.closing) {
            cb(server, NULL, -1, op->user_data);
        }
        return;
    }

    /* Inherit listen socket properties. */
    setsockopt(
        op->accept_sock,
        SOL_SOCKET,
        SO_UPDATE_ACCEPT_CONTEXT,
        (char*)&server->sock,
        sizeof(server->sock));

    /* Create a new tcp handle for the accepted client. */
    netkit_tcp_t* client = (netkit_tcp_t*)calloc(1, sizeof(netkit_tcp_t));
    if (!client) {
        closesocket(op->accept_sock);
        op->accept_sock = INVALID_SOCKET;
        if (cb) {
            cb(server, NULL, -1, op->user_data);
        }
    } else {
        client->base.type = _HANDLE_TCP;
        client->base.loop = server->base.loop;
        client->sock      = op->accept_sock;
        op->accept_sock   = INVALID_SOCKET;

        _loop_associate(server->base.loop, client->sock);
        _loop_handle_register(server->base.loop);

        if (cb) {
            cb(server, client, 0, op->user_data);
        }
    }

    /* Post next accept if server is still listening. */
    if (!server->base.closing) {
        _tcp_post_accept(server);
    }
}

void _tcp_on_connect(_op_t* op, BOOL success) {
    netkit_tcp_t* tcp = (netkit_tcp_t*)op->handle;
    netkit_tcp_connect_fn_t cb = (netkit_tcp_connect_fn_t)op->cb;

    if (success) {
        /* Enable normal send/recv after ConnectEx. */
        setsockopt(tcp->sock, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0);
    }

    if (cb) {
        cb(tcp, success ? 0 : -1, op->user_data);
    }
}

void _tcp_on_read(_op_t* op, DWORD bytes, BOOL success) {
    netkit_tcp_t* tcp = (netkit_tcp_t*)op->handle;
    netkit_tcp_read_fn_t cb = (netkit_tcp_read_fn_t)op->cb;

    if (!success || bytes == 0) {
        if (cb) {
            int32_t err = (bytes == 0) ? NETKIT_ERR_EOF
                                       : netkit_err_from_wsa();
            cb(tcp, NULL, err, op->user_data);
        }
        return;
    }

    if (cb) {
        cb(tcp, op->read_buf, (int32_t)bytes, op->user_data);
    }
}

void _tcp_on_write(_op_t* op, BOOL success) {
    netkit_tcp_t* tcp = (netkit_tcp_t*)op->handle;
    netkit_tcp_write_fn_t cb = (netkit_tcp_write_fn_t)op->cb;

    if (cb) {
        cb(tcp, success ? 0 : -1, op->user_data);
    }
}
