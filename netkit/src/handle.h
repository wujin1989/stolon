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

#include <stdbool.h>
#include <stdint.h>

typedef enum _handle_type_e {
    _HANDLE_TCP   = 1,
    _HANDLE_UDP   = 2,
    _HANDLE_TIMER = 3
} _handle_type_t;

typedef struct _handle_s {
    _handle_type_t    type;           /**< Handle type discriminator. */
    netkit_loop_t*    loop;           /**< Owning event loop. */
    bool              closing;        /**< True if close has been requested. */
    bool              in_dispatch;    /**< True while inside a dispatch handler. */
    int32_t           pending_ops;    /**< Number of outstanding async ops. */
    netkit_close_fn_t close_cb;       /**< User close callback. */
    void*             close_data;     /**< User data for close callback. */
} _handle_t;
