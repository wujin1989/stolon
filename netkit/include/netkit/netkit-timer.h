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
 * @brief Create a new timer handle.
 *
 * @param loop  The event loop to attach to.
 *
 * @return New timer, or NULL on failure.
 */
extern netkit_timer_t* netkit_timer_create(netkit_loop_t* loop);

/**
 * @brief Start the timer.
 *
 * @param timer       The timer handle.
 * @param timeout_ms  Initial delay in milliseconds.
 * @param repeat_ms   Repeat interval in ms. 0 for one-shot.
 * @param cb          Callback fired on expiration.
 * @param data        User data passed to callback.
 *
 * @return 0 on success, -1 on failure.
 */
extern int netkit_timer_start(
    netkit_timer_t* timer,
    uint64_t timeout_ms,
    uint64_t repeat_ms,
    netkit_timer_timeout_fn_t cb,
    void* data);

/**
 * @brief Stop the timer. It will not fire again until restarted.
 *
 * @param timer  The timer handle.
 */
extern void netkit_timer_stop(netkit_timer_t* timer);

/**
 * @brief Close the timer and release resources.
 *
 * The close callback fires after the timer is fully cleaned up.
 *
 * @param timer  The timer handle.
 * @param cb     Close callback (may be NULL).
 * @param data   User data for close callback.
 */
extern void netkit_timer_close(
    netkit_timer_t* timer,
    netkit_close_fn_t cb,
    void* data);
