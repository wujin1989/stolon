#include "netkit.h"
#include "assert.h"

static void test_create_destroy(void) {
    netkit_loop_t* loop = netkit_loop_create();
    ASSERT(loop != NULL);
    netkit_loop_destroy(loop);
}

static void test_destroy_null(void) {
    netkit_loop_destroy(NULL);
}

static void test_run_empty_exits(void) {
    /* A loop with no active handles should return immediately. */
    netkit_loop_t* loop = netkit_loop_create();
    ASSERT(loop != NULL);
    int rc = netkit_loop_run(loop);
    ASSERT(rc == 0);
    netkit_loop_destroy(loop);
}

static void test_run_null(void) {
    int rc = netkit_loop_run(NULL);
    ASSERT(rc == -1);
}

int main(void) {
    test_create_destroy();
    test_destroy_null();
    test_run_empty_exits();
    test_run_null();
    return 0;
}
