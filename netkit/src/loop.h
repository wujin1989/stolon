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

#include "netkit/netkit-types.h"

#include "heap.h"

#include <winsock2.h>
#include <stdbool.h>
#include <stdint.h>

struct netkit_loop_s {
    HANDLE   iocp;            /**< IOCP completion port handle. */
    bool     running;         /**< False when stop has been requested. */
    int32_t  active_handles;  /**< Number of active (non-closing) handles. */
    _heap_t  timers;          /**< Min-heap of pending timers. */
};

/**
 * @brief Register a handle with the loop (increment active count).
 *
 * @param loop  The event loop.
 */
extern void _loop_handle_register(netkit_loop_t* loop);

/**
 * @brief Unregister a handle from the loop (decrement active count).
 *
 * @param loop  The event loop.
 */
extern void _loop_handle_unregister(netkit_loop_t* loop);

/**
 * @brief Associate a socket with the loop IOCP handle.
 *
 * @param loop  The event loop.
 * @param sock  The socket to associate.
 *
 * @return 0 on success, -1 on failure.
 */
extern int _loop_associate(netkit_loop_t* loop, SOCKET sock);
