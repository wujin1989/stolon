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

#include "netkit/netkit-timer.h"

#include "netkit-timer.h"
#include "netkit-loop.h"
#include "heap.h"

#include <windows.h>
#include <stdlib.h>

netkit_timer_t* netkit_timer_create(netkit_loop_t* loop) {
    if (!loop) {
        return NULL;
    }
    netkit_timer_t* timer = (netkit_timer_t*)calloc(1, sizeof(netkit_timer_t));
    if (!timer) {
        return NULL;
    }
    timer->base.type = _HANDLE_TIMER;
    timer->base.loop = loop;
    _loop_handle_register(loop);
    return timer;
}

int netkit_timer_start(
    netkit_timer_t* timer,
    uint64_t timeout_ms,
    uint64_t repeat_ms,
    netkit_timer_timeout_fn_t cb,
    void* data) {

    if (!timer || !cb) {
        return -1;
    }
    if (timer->base.closing) {
        return -1;
    }

    /* If already active, remove from heap first. */
    if (timer->active) {
        _heap_remove(&timer->base.loop->timers, timer);
    }

    timer->timeout_ms = timeout_ms;
    timer->repeat_ms  = repeat_ms;
    timer->cb         = cb;
    timer->user_data  = data;
    timer->active     = true;

    uint64_t expiry = GetTickCount64() + timeout_ms;
    if (_heap_insert(&timer->base.loop->timers, expiry, timer) != 0) {
        timer->active = false;
        return -1;
    }
    return 0;
}

void netkit_timer_stop(netkit_timer_t* timer) {
    if (!timer || !timer->active) {
        return;
    }
    _heap_remove(&timer->base.loop->timers, timer);
    timer->active = false;
}

void netkit_timer_close(
    netkit_timer_t* timer,
    netkit_close_fn_t cb,
    void* data) {

    if (!timer || timer->base.closing) {
        return;
    }
    timer->base.closing    = true;
    timer->base.close_cb   = cb;
    timer->base.close_data = data;

    /* Remove from heap if active. */
    if (timer->active) {
        _heap_remove(&timer->base.loop->timers, timer);
        timer->active = false;
    }

    /* Timers have no pending IOCP ops, so close immediately. */
    if (cb) {
        cb(timer, data);
    }
    _loop_handle_unregister(timer->base.loop);
    free(timer);
}

void _timer_fire(netkit_timer_t* timer) {
    if (!timer || !timer->active) {
        return;
    }

    /* Invoke user callback. */
    if (timer->cb) {
        timer->cb(timer, timer->user_data);
    }

    /* Re-schedule if repeating and not closed during callback. */
    if (timer->repeat_ms > 0 && !timer->base.closing) {
        uint64_t expiry = GetTickCount64() + timer->repeat_ms;
        if (_heap_insert(&timer->base.loop->timers, expiry, timer) != 0) {
            timer->active = false;
        }
    } else {
        timer->active = false;
    }
}
