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

#include "handle.h"

#include <stdint.h>
#include <stdbool.h>

struct netkit_timer_s {
    _handle_t                 base;        /**< Embedded handle base. */
    uint64_t                  timeout_ms;  /**< Initial delay. */
    uint64_t                  repeat_ms;   /**< Repeat interval (0 = one-shot). */
    bool                      active;      /**< True if timer is scheduled. */
    netkit_timer_timeout_fn_t cb;          /**< User timeout callback. */
    void*                     user_data;   /**< User data for callback. */
};

/**
 * @brief Fire a timer that has expired. Called by the loop.
 *
 * Invokes the user callback. If repeat_ms > 0, re-inserts into the heap.
 *
 * @param timer  The timer handle.
 */
extern void _timer_fire(netkit_timer_t* timer);
