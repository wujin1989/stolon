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

#include "netkit/netkit-loop.h"

#include "netkit-loop.h"
#include "netkit-timer.h"
#include "netkit-tcp.h"
#include "netkit-udp.h"
#include "handle.h"
#include "op.h"

#include <winsock2.h>
#include <stdlib.h>

/* Get the IOCP timeout based on the nearest timer expiration. */
static DWORD _loop_calc_timeout(netkit_loop_t* loop) {
    _heap_entry_t entry;
    if (_heap_peek(&loop->timers, &entry) != 0) {
        return INFINITE;
    }
    uint64_t now = GetTickCount64();
    if (entry.key <= now) {
        return 0;
    }
    uint64_t diff = entry.key - now;
    if (diff > (uint64_t)0xFFFFFFFE) {
        return 0xFFFFFFFE;
    }
    return (DWORD)diff;
}

/* Fire all expired timers. */
static void _loop_process_timers(netkit_loop_t* loop) {
    uint64_t now = GetTickCount64();
    for (;;) {
        _heap_entry_t entry;
        if (_heap_peek(&loop->timers, &entry) != 0) {
            break;
        }
        if (entry.key > now) {
            break;
        }
        _heap_pop(&loop->timers, &entry);
        _timer_fire((netkit_timer_t*)entry.data);
    }
}

/* Process a single IOCP completion. */
static void _loop_dispatch(
    _op_t* op,
    DWORD bytes_transferred,
    BOOL success) {

    _handle_t* h = (_handle_t*)op->handle;
    h->pending_ops--;

    switch (op->type) {
    case _OP_ACCEPT:
        _tcp_on_accept(op, bytes_transferred, success);
        break;
    case _OP_CONNECT:
        _tcp_on_connect(op, success);
        break;
    case _OP_READ:
        _tcp_on_read(op, bytes_transferred, success);
        break;
    case _OP_WRITE:
        _tcp_on_write(op, success);
        break;
    case _OP_RECV:
        _udp_on_recv(op, bytes_transferred, success);
        break;
    case _OP_SEND:
        _udp_on_send(op, success);
        break;
    }

    /* If handle is closing and no more pending ops, finish the close. */
    if (h->closing && h->pending_ops == 0) {
        if (h->close_cb) {
            h->close_cb(op->handle, h->close_data);
        }
        _loop_handle_unregister(h->loop);
        free(op->handle);
    }

    _op_destroy(op);
}

netkit_loop_t* netkit_loop_create(void) {
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        return NULL;
    }

    netkit_loop_t* loop = (netkit_loop_t*)calloc(1, sizeof(netkit_loop_t));
    if (!loop) {
        WSACleanup();
        return NULL;
    }

    loop->iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
    if (!loop->iocp) {
        free(loop);
        WSACleanup();
        return NULL;
    }

    loop->running        = true;
    loop->active_handles = 0;
    _heap_init(&loop->timers);

    return loop;
}

int netkit_loop_run(netkit_loop_t* loop) {
    if (!loop) {
        return -1;
    }

    while (loop->running && loop->active_handles > 0) {
        DWORD timeout = _loop_calc_timeout(loop);
        DWORD bytes   = 0;
        ULONG_PTR key = 0;
        OVERLAPPED* ov = NULL;

        BOOL ok = GetQueuedCompletionStatus(
            loop->iocp,
            &bytes,
            &key,
            &ov,
            timeout);

        /* Process expired timers regardless of IOCP result. */
        _loop_process_timers(loop);

        if (ov) {
            _op_t* op = (_op_t*)ov;
            _loop_dispatch(op, bytes, ok);
        }
    }

    return 0;
}

void netkit_loop_stop(netkit_loop_t* loop) {
    if (!loop) {
        return;
    }
    loop->running = false;
    /* Post a dummy completion to wake up GetQueuedCompletionStatus. */
    PostQueuedCompletionStatus(loop->iocp, 0, 0, NULL);
}

void netkit_loop_destroy(netkit_loop_t* loop) {
    if (!loop) {
        return;
    }
    if (loop->iocp) {
        CloseHandle(loop->iocp);
    }
    _heap_deinit(&loop->timers);
    free(loop);
    WSACleanup();
}

void _loop_handle_register(netkit_loop_t* loop) {
    loop->active_handles++;
}

void _loop_handle_unregister(netkit_loop_t* loop) {
    loop->active_handles--;
}

int _loop_associate(netkit_loop_t* loop, SOCKET sock) {
    HANDLE result = CreateIoCompletionPort(
        (HANDLE)sock,
        loop->iocp,
        0,
        0);
    return result ? 0 : -1;
}
