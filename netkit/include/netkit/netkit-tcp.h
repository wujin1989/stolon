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
 * @brief Create a new TCP handle.
 *
 * @param loop  The event loop.
 *
 * @return New TCP handle, or NULL on failure.
 */
extern netkit_tcp_t* netkit_tcp_create(netkit_loop_t* loop);

/**
 * @brief Bind the TCP socket to a local address.
 *
 * @param tcp   The TCP handle.
 * @param ip    IP address string (e.g. "127.0.0.1").
 * @param port  Port number.
 *
 * @return 0 on success, -1 on failure.
 */
extern int netkit_tcp_bind(netkit_tcp_t* tcp, const char* ip, int32_t port);

/**
 * @brief Start listening for incoming connections.
 *
 * @param tcp      The TCP handle (must be bound).
 * @param backlog  Listen backlog size.
 * @param cb       Callback invoked for each accepted connection.
 * @param data     User data for callback.
 *
 * @return 0 on success, -1 on failure.
 */
extern int netkit_tcp_listen(
    netkit_tcp_t* tcp,
    int32_t backlog,
    netkit_tcp_accept_fn_t cb,
    void* data);

/**
 * @brief Initiate an async connection to a remote address.
 *
 * @param tcp   The TCP handle.
 * @param ip    Remote IP address.
 * @param port  Remote port.
 * @param cb    Callback invoked when connection completes.
 * @param data  User data for callback.
 *
 * @return 0 on success, -1 on failure.
 */
extern int netkit_tcp_connect(
    netkit_tcp_t* tcp,
    const char* ip,
    int32_t port,
    netkit_tcp_connect_fn_t cb,
    void* data);

/**
 * @brief Start reading data from the TCP connection.
 *
 * @param tcp   The TCP handle.
 * @param cb    Callback invoked when data arrives. nread < 0 means error/EOF.
 * @param data  User data for callback.
 *
 * @return 0 on success, -1 on failure.
 */
extern int netkit_tcp_read(
    netkit_tcp_t* tcp,
    netkit_tcp_read_fn_t cb,
    void* data);

/**
 * @brief Write data to the TCP connection.
 *
 * @param tcp   The TCP handle.
 * @param buf   Data buffer.
 * @param len   Number of bytes to write.
 * @param cb    Callback invoked when write completes.
 * @param data  User data for callback.
 *
 * @return 0 on success, -1 on failure.
 */
extern int netkit_tcp_write(
    netkit_tcp_t* tcp,
    const void* buf,
    int32_t len,
    netkit_tcp_write_fn_t cb,
    void* data);

/**
 * @brief Close the TCP handle asynchronously.
 *
 * Pending operations complete with NETKIT_ERR_CANCELLED. The close callback
 * fires after all operations finish and the socket is closed.
 *
 * @param tcp   The TCP handle.
 * @param cb    Close callback (may be NULL).
 * @param data  User data for close callback.
 */
extern void netkit_tcp_close(
    netkit_tcp_t* tcp,
    netkit_close_fn_t cb,
    void* data);
