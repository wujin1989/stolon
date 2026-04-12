#include "netkit.h"
#include "assert.h"

#include <string.h>

#define TEST_PORT 19876
#define TEST_MSG  "hello netkit"

static netkit_loop_t* _loop = NULL;
static netkit_timer_t* _safety = NULL;
static int32_t _test_passed = 0;

static void _on_client_close(void* handle, void* data) {
    (void)handle;
    (void)data;
}

static void _on_server_read(
    netkit_tcp_t* tcp,
    const void* buf,
    int32_t nread,
    void* data) {

    (void)data;
    if (nread < 0) {
        netkit_tcp_close(tcp, _on_client_close, NULL);
        return;
    }
    ASSERT(nread == (int32_t)strlen(TEST_MSG));
    ASSERT(memcmp(buf, TEST_MSG, (size_t)nread) == 0);
    _test_passed = 1;
    netkit_tcp_close(tcp, _on_client_close, NULL);
    /* Cancel safety timer so loop exits promptly. */
    netkit_timer_close(_safety, NULL, NULL);
    _safety = NULL;
}

static void _on_write_done(netkit_tcp_t* tcp, int status, void* data) {
    (void)data;
    ASSERT(status == 0);
    netkit_tcp_close(tcp, _on_client_close, NULL);
}

static void _on_connect(netkit_tcp_t* tcp, int status, void* data) {
    (void)data;
    ASSERT(status == 0);
    netkit_tcp_write(
        tcp, TEST_MSG, (int32_t)strlen(TEST_MSG), _on_write_done, NULL);
}

static void _on_accept(
    netkit_tcp_t* server,
    netkit_tcp_t* client,
    int status,
    void* data) {

    (void)data;
    ASSERT(status == 0);
    ASSERT(client != NULL);

    netkit_tcp_read(client, _on_server_read, NULL);

    /* Close server after first accept. */
    netkit_tcp_close(server, NULL, NULL);
}

static void _on_safety_timeout(netkit_timer_t* timer, void* data) {
    (void)data;
    netkit_timer_close(timer, NULL, NULL);
    _safety = NULL;
    netkit_loop_stop(_loop);
}

static void test_tcp_echo(void) {
    _loop = netkit_loop_create();
    ASSERT(_loop != NULL);

    /* Safety timer. */
    _safety = netkit_timer_create(_loop);
    ASSERT(_safety != NULL);
    netkit_timer_start(_safety, 5000, 0, _on_safety_timeout, NULL);

    /* Server. */
    netkit_tcp_t* server = netkit_tcp_create(_loop);
    ASSERT(server != NULL);
    ASSERT(netkit_tcp_bind(server, "127.0.0.1", TEST_PORT) == 0);
    ASSERT(netkit_tcp_listen(server, 1, _on_accept, NULL) == 0);

    /* Client. */
    netkit_tcp_t* client = netkit_tcp_create(_loop);
    ASSERT(client != NULL);
    ASSERT(netkit_tcp_connect(
        client, "127.0.0.1", TEST_PORT, _on_connect, NULL) == 0);

    netkit_loop_run(_loop);
    netkit_loop_destroy(_loop);

    ASSERT(_test_passed == 1);
}

int main(void) {
    test_tcp_echo();
    return 0;
}
