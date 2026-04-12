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

#include "netkit-types.h"

#include <stdint.h>

/**
 * @brief Create a new UDP handle.
 *
 * @param loop  The event loop.
 *
 * @return New UDP handle, or NULL on failure.
 */
extern netkit_udp_t* netkit_udp_create(netkit_loop_t* loop);

/**
 * @brief Bind the UDP socket to a local address.
 *
 * @param udp   The UDP handle.
 * @param ip    IP address string.
 * @param port  Port number.
 *
 * @return 0 on success, -1 on failure.
 */
extern int netkit_udp_bind(netkit_udp_t* udp, const char* ip, int32_t port);

/**
 * @brief Start receiving datagrams.
 *
 * @param udp   The UDP handle (must be bound).
 * @param cb    Callback invoked for each received datagram.
 * @param data  User data for callback.
 *
 * @return 0 on success, -1 on failure.
 */
extern int netkit_udp_recv(
    netkit_udp_t* udp,
    netkit_udp_recv_fn_t cb,
    void* data);

/**
 * @brief Send a datagram to a remote address.
 *
 * @param udp   The UDP handle.
 * @param buf   Data buffer.
 * @param len   Number of bytes to send.
 * @param ip    Destination IP address.
 * @param port  Destination port.
 * @param cb    Callback invoked when send completes.
 * @param data  User data for callback.
 *
 * @return 0 on success, -1 on failure.
 */
extern int netkit_udp_send(
    netkit_udp_t* udp,
    const void* buf,
    int32_t len,
    const char* ip,
    int32_t port,
    netkit_udp_send_fn_t cb,
    void* data);

/**
 * @brief Close the UDP handle asynchronously.
 *
 * @param udp   The UDP handle.
 * @param cb    Close callback (may be NULL).
 * @param data  User data for close callback.
 */
extern void netkit_udp_close(
    netkit_udp_t* udp,
    netkit_close_fn_t cb,
    void* data);
