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

_Pragma("once")

#include <stdint.h>

/* Opaque type forward declarations */
typedef struct netkit_loop_s  netkit_loop_t;
typedef struct netkit_tcp_s   netkit_tcp_t;
typedef struct netkit_udp_s   netkit_udp_t;
typedef struct netkit_timer_s netkit_timer_t;

/* Callback typedefs */
typedef void (*netkit_tcp_accept_fn_t)(
    netkit_tcp_t* server,
    netkit_tcp_t* client,
    int status,
    void* data);

typedef void (*netkit_tcp_connect_fn_t)(
    netkit_tcp_t* tcp,
    int status,
    void* data);

typedef void (*netkit_tcp_read_fn_t)(
    netkit_tcp_t* tcp,
    const void* buf,
    int32_t nread,
    void* data);

typedef void (*netkit_tcp_write_fn_t)(
    netkit_tcp_t* tcp,
    int status,
    void* data);

typedef void (*netkit_udp_recv_fn_t)(
    netkit_udp_t* udp,
    const void* buf,
    int32_t nread,
    const char* addr,
    int32_t port,
    void* data);

typedef void (*netkit_udp_send_fn_t)(
    netkit_udp_t* udp,
    int status,
    void* data);

typedef void (*netkit_timer_timeout_fn_t)(
    netkit_timer_t* timer,
    void* data);

typedef void (*netkit_close_fn_t)(
    void* handle,
    void* data);
