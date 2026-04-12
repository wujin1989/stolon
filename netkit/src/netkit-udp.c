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

#include "netkit/netkit-udp.h"
#include "netkit/netkit-err.h"

#include "netkit-udp.h"
#include "netkit-loop.h"
#include "netkit-err.h"

#include <ws2tcpip.h>
#include <stdlib.h>
#include <string.h>

/* Extended op for UDP recv that stores the sender address. */
typedef struct _udp_recv_op_s {
    _op_t              base;       /**< Base op (OVERLAPPED first). */
    struct sockaddr_in from_addr;  /**< Sender address filled by WSARecvFrom. */
    int                from_len;   /**< Size of from_addr. */
} _udp_recv_op_t;

/* Extended op for UDP send that stores the destination address. */
typedef struct _udp_send_op_s {
    _op_t              base;     /**< Base op (OVERLAPPED first). */
    struct sockaddr_in to_addr;  /**< Destination address. */
} _udp_send_op_t;

netkit_udp_t* netkit_udp_create(netkit_loop_t* loop) {
    if (!loop) {
        return NULL;
    }

    SOCKET sock = WSASocketW(
        AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (sock == INVALID_SOCKET) {
        return NULL;
    }

    netkit_udp_t* udp = (netkit_udp_t*)calloc(1, sizeof(netkit_udp_t));
    if (!udp) {
        closesocket(sock);
        return NULL;
    }

    udp->base.type = _HANDLE_UDP;
    udp->base.loop = loop;
    udp->sock      = sock;

    if (_loop_associate(loop, sock) != 0) {
        closesocket(sock);
        free(udp);
        return NULL;
    }

    _loop_handle_register(loop);
    return udp;
}

int netkit_udp_bind(netkit_udp_t* udp, const char* ip, int32_t port) {
    if (!udp || !ip || udp->base.closing) {
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons((uint16_t)port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1) {
        return -1;
    }

    if (bind(udp->sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        return -1;
    }
    return 0;
}

int netkit_udp_recv(
    netkit_udp_t* udp,
    netkit_udp_recv_fn_t cb,
    void* data) {

    if (!udp || !cb || udp->base.closing) {
        return -1;
    }

    _udp_recv_op_t* rop = (_udp_recv_op_t*)calloc(1, sizeof(_udp_recv_op_t));
    if (!rop) {
        return -1;
    }

    rop->base.type        = _OP_RECV;
    rop->base.handle      = udp;
    rop->base.cb          = (void*)cb;
    rop->base.user_data   = data;
    rop->base.accept_sock = INVALID_SOCKET;
    rop->base.wsa_buf.buf = (char*)rop->base.read_buf;
    rop->base.wsa_buf.len = OP_READ_BUF_SIZE;
    rop->from_len         = (int)sizeof(rop->from_addr);

    DWORD flags = 0;
    int rc = WSARecvFrom(
        udp->sock,
        &rop->base.wsa_buf,
        1,
        NULL,
        &flags,
        (struct sockaddr*)&rop->from_addr,
        &rop->from_len,
        &rop->base.overlapped,
        NULL);

    if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        free(rop);
        return -1;
    }

    udp->base.pending_ops++;
    return 0;
}

int netkit_udp_send(
    netkit_udp_t* udp,
    const void* buf,
    int32_t len,
    const char* ip,
    int32_t port,
    netkit_udp_send_fn_t cb,
    void* data) {

    if (!udp || !buf || len <= 0 || !ip || udp->base.closing) {
        return -1;
    }

    _udp_send_op_t* sop = (_udp_send_op_t*)calloc(1, sizeof(_udp_send_op_t));
    if (!sop) {
        return -1;
    }

    sop->base.type        = _OP_SEND;
    sop->base.handle      = udp;
    sop->base.cb          = (void*)cb;
    sop->base.user_data   = data;
    sop->base.accept_sock = INVALID_SOCKET;
    sop->base.wsa_buf.buf = (char*)buf;
    sop->base.wsa_buf.len = (ULONG)len;

    memset(&sop->to_addr, 0, sizeof(sop->to_addr));
    sop->to_addr.sin_family = AF_INET;
    sop->to_addr.sin_port   = htons((uint16_t)port);
    if (inet_pton(AF_INET, ip, &sop->to_addr.sin_addr) != 1) {
        free(sop);
        return -1;
    }

    int rc = WSASendTo(
        udp->sock,
        &sop->base.wsa_buf,
        1,
        NULL,
        0,
        (struct sockaddr*)&sop->to_addr,
        sizeof(sop->to_addr),
        &sop->base.overlapped,
        NULL);

    if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        free(sop);
        return -1;
    }

    udp->base.pending_ops++;
    return 0;
}

void netkit_udp_close(
    netkit_udp_t* udp,
    netkit_close_fn_t cb,
    void* data) {

    if (!udp || udp->base.closing) {
        return;
    }

    udp->base.closing    = true;
    udp->base.close_cb   = cb;
    udp->base.close_data = data;

    if (udp->sock != INVALID_SOCKET) {
        CancelIoEx((HANDLE)udp->sock, NULL);
        closesocket(udp->sock);
        udp->sock = INVALID_SOCKET;
    }

    if (udp->base.pending_ops == 0 && !udp->base.in_dispatch) {
        if (cb) {
            cb(udp, data);
        }
        _loop_handle_unregister(udp->base.loop);
        free(udp);
    }
}

void _udp_on_recv(_op_t* op, DWORD bytes, BOOL success) {
    netkit_udp_t* udp = (netkit_udp_t*)op->handle;
    netkit_udp_recv_fn_t cb = (netkit_udp_recv_fn_t)op->cb;
    _udp_recv_op_t* rop = (_udp_recv_op_t*)op;

    if (!success || bytes == 0) {
        if (cb && !udp->base.closing) {
            cb(udp, NULL, netkit_err_from_wsa(), NULL, 0, op->user_data);
        }
        return;
    }

    /* Convert sender address to string. */
    char addr_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &rop->from_addr.sin_addr, addr_str, sizeof(addr_str));
    int32_t from_port = (int32_t)ntohs(rop->from_addr.sin_port);

    if (cb) {
        cb(udp, op->read_buf, (int32_t)bytes, addr_str, from_port, op->user_data);
    }
}

void _udp_on_send(_op_t* op, BOOL success) {
    netkit_udp_t* udp = (netkit_udp_t*)op->handle;
    netkit_udp_send_fn_t cb = (netkit_udp_send_fn_t)op->cb;

    if (cb) {
        cb(udp, success ? 0 : -1, op->user_data);
    }
}
