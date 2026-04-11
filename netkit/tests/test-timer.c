#include "netkit.h"
#include "assert.h"

static int32_t _fire_count = 0;

static void _on_timeout(netkit_timer_t* timer, void* data) {
    (void)data;
    _fire_count++;
    /* Stop after 3 fires for repeat test. */
    if (_fire_count >= 3) {
        netkit_timer_stop(timer);
        netkit_timer_close(timer, NULL, NULL);
    }
}

static void _on_single_timeout(netkit_timer_t* timer, void* data) {
    int32_t* flag = (int32_t*)data;
    *flag = 1;
    netkit_timer_close(timer, NULL, NULL);
}

static void _on_safety_timeout(netkit_timer_t* timer, void* data) {
    /* Safety timer: if we get here, the test is hanging. Force stop. */
    netkit_loop_t** loop_ptr = (netkit_loop_t**)data;
    netkit_timer_close(timer, NULL, NULL);
    netkit_loop_stop(*loop_ptr);
}

static void test_single_shot(void) {
    netkit_loop_t* loop = netkit_loop_create();
    ASSERT(loop != NULL);

    int32_t fired = 0;
    netkit_timer_t* timer = netkit_timer_create(loop);
    ASSERT(timer != NULL);
    ASSERT(netkit_timer_start(timer, 10, 0, _on_single_timeout, &fired) == 0);

    /* Safety timer to prevent hang. */
    netkit_timer_t* safety = netkit_timer_create(loop);
    ASSERT(safety != NULL);
    ASSERT(netkit_timer_start(safety, 2000, 0, _on_safety_timeout, &loop) == 0);

    netkit_loop_run(loop);
    ASSERT(fired == 1);
    netkit_loop_destroy(loop);
}

static void test_repeat(void) {
    netkit_loop_t* loop = netkit_loop_create();
    ASSERT(loop != NULL);

    _fire_count = 0;
    netkit_timer_t* timer = netkit_timer_create(loop);
    ASSERT(timer != NULL);
    ASSERT(netkit_timer_start(timer, 10, 10, _on_timeout, NULL) == 0);

    /* Safety timer. */
    netkit_timer_t* safety = netkit_timer_create(loop);
    ASSERT(safety != NULL);
    ASSERT(netkit_timer_start(safety, 2000, 0, _on_safety_timeout, &loop) == 0);

    netkit_loop_run(loop);
    ASSERT(_fire_count >= 3);
    netkit_loop_destroy(loop);
}

static void _on_stop_timeout(netkit_timer_t* timer, void* data) {
    (void)data;
    (void)timer;
    /* Should not be called. */
    ASSERT(0);
}

static void test_stop_prevents_fire(void) {
    netkit_loop_t* loop = netkit_loop_create();
    ASSERT(loop != NULL);

    netkit_timer_t* timer = netkit_timer_create(loop);
    ASSERT(timer != NULL);
    ASSERT(netkit_timer_start(timer, 50, 0, _on_stop_timeout, NULL) == 0);
    netkit_timer_stop(timer);
    netkit_timer_close(timer, NULL, NULL);

    /* Run briefly to confirm nothing fires. */
    netkit_timer_t* done = netkit_timer_create(loop);
    ASSERT(done != NULL);
    int32_t flag = 0;
    ASSERT(netkit_timer_start(done, 100, 0, _on_single_timeout, &flag) == 0);

    netkit_loop_run(loop);
    ASSERT(flag == 1);
    netkit_loop_destroy(loop);
}

int main(void) {
    test_single_shot();
    test_repeat();
    test_stop_prevents_fire();
    return 0;
}
