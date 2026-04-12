#include "netkit.h"
#include "assert.h"

#include <string.h>

#define TEST_PORT 19877
#define TEST_MSG  "hello udp"

static netkit_loop_t* _loop = NULL;
static netkit_timer_t* _safety = NULL;

static void _on_safety_timeout(netkit_timer_t* timer, void* data) {
    (void)data;
    netkit_timer_close(timer, NULL, NULL);
    _safety = NULL;
    netkit_loop_stop(_loop);
}

static void _on_recv(
    netkit_udp_t* udp,
    const void* buf,
    int32_t nread,
    const char* addr,
    int32_t port,
    void* data) {

    (void)addr;
    (void)port;
    (void)data;

    ASSERT(nread == (int32_t)strlen(TEST_MSG));
    ASSERT(memcmp(buf, TEST_MSG, (size_t)nread) == 0);
    netkit_udp_close(udp, NULL, NULL);
    /* Cancel safety timer. */
    if (_safety) {
        netkit_timer_close(_safety, NULL, NULL);
        _safety = NULL;
    }
}

static void _on_send(netkit_udp_t* udp, int status, void* data) {
    (void)data;
    ASSERT(status == 0);
    netkit_udp_close(udp, NULL, NULL);
}

static void test_udp_send_recv(void) {
    _loop = netkit_loop_create();
    ASSERT(_loop != NULL);

    /* Safety timer. */
    _safety = netkit_timer_create(_loop);
    ASSERT(_safety != NULL);
    netkit_timer_start(_safety, 5000, 0, _on_safety_timeout, NULL);

    /* Receiver. */
    netkit_udp_t* receiver = netkit_udp_create(_loop);
    ASSERT(receiver != NULL);
    ASSERT(netkit_udp_bind(receiver, "127.0.0.1", TEST_PORT) == 0);
    ASSERT(netkit_udp_recv(receiver, _on_recv, NULL) == 0);

    /* Sender. */
    netkit_udp_t* sender = netkit_udp_create(_loop);
    ASSERT(sender != NULL);
    ASSERT(netkit_udp_send(
        sender,
        TEST_MSG,
        (int32_t)strlen(TEST_MSG),
        "127.0.0.1",
        TEST_PORT,
        _on_send,
        NULL) == 0);

    netkit_loop_run(_loop);
    netkit_loop_destroy(_loop);
}

int main(void) {
    test_udp_send_recv();
    return 0;
}
