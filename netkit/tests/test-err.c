#include "netkit.h"
#include "assert.h"

#include <string.h>

static void test_strerror_success(void) {
    const char* msg = netkit_err_strerror(0);
    ASSERT(msg != NULL);
    ASSERT(strcmp(msg, "success") == 0);
}

static void test_strerror_nomem(void) {
    const char* msg = netkit_err_strerror(NETKIT_ERR_NOMEM);
    ASSERT(msg != NULL);
    ASSERT(strcmp(msg, "out of memory") == 0);
}

static void test_strerror_eof(void) {
    const char* msg = netkit_err_strerror(NETKIT_ERR_EOF);
    ASSERT(msg != NULL);
    ASSERT(strcmp(msg, "end of stream") == 0);
}

static void test_strerror_unknown_code(void) {
    const char* msg = netkit_err_strerror(-1234);
    ASSERT(msg != NULL);
    ASSERT(strcmp(msg, "unknown error") == 0);
}

int main(void) {
    test_strerror_success();
    test_strerror_nomem();
    test_strerror_eof();
    test_strerror_unknown_code();
    return 0;
}
