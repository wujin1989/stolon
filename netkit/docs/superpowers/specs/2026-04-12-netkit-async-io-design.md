# netkit Async I/O Library Design

## Summary

netkit is a Windows-only C library providing high-performance asynchronous I/O based on IOCP. It targets TCP, UDP, and timers with a single-threaded event loop model. It is a self-use infrastructure library.

## Architecture

Three-layer structure:

- **nk_loop_t** — Event loop. Holds the IOCP handle, manages all handle lifetimes. Single-threaded, `nk_loop_run()` blocks until no active handles remain or `nk_loop_stop()` is called.
- **nk_handle_t** — Internal base type (not exposed to users). Contains type identifier, loop pointer, closing flag. Concrete types: `nk_tcp_t`, `nk_udp_t`, `nk_timer_t`, each embedding `nk_handle_t` as the first field.
- **nk_op_t** — Async operation. Embeds `OVERLAPPED` as the first field (for pointer casting from IOCP completion). Contains operation type, callback pointer, `void* user_data`, and buffer info (`WSABUF` etc.). Created per read/write/connect/accept, freed after completion.

### Data Flow

1. User calls e.g. `nk_tcp_read(tcp, cb, data)`
2. Library creates `nk_op_t`, submits `WSARecv` to IOCP
3. `nk_loop_run()` calls `GetQueuedCompletionStatus()`
4. Casts `OVERLAPPED*` back to `nk_op_t*`
5. Dispatches by `op_type` to handler
6. Handler invokes user callback, frees op
7. Checks timer min-heap, fires expired timers

### Timer Implementation

Timers are managed via a min-heap sorted by expiration time. Each iteration of the loop uses the heap-top's remaining time as the `GetQueuedCompletionStatus` timeout parameter.

### TCP Accept

Uses `AcceptEx` for async accept. Pre-creates an accept socket stored in the op. On completion, calls `setsockopt(SO_UPDATE_ACCEPT_CONTEXT)` to inherit listener socket properties.

## Public API

Prefix: `nk_`. All public functions return `int` (0 = success, negative = error).
Memory: library-allocated (`nk_*_new()` / internal free on close).
Callback style: function pointer + `void* user_data`.

### Loop

```c
nk_loop_t*  nk_loop_new(void);
int         nk_loop_run(nk_loop_t* loop);
void        nk_loop_stop(nk_loop_t* loop);
void        nk_loop_free(nk_loop_t* loop);
```

### TCP

```c
nk_tcp_t*   nk_tcp_new(nk_loop_t* loop);
int         nk_tcp_bind(nk_tcp_t* tcp, const char* ip, int port);
int         nk_tcp_listen(nk_tcp_t* tcp, int backlog, nk_accept_cb cb, void* data);
int         nk_tcp_connect(nk_tcp_t* tcp, const char* ip, int port, nk_connect_cb cb, void* data);
int         nk_tcp_read(nk_tcp_t* tcp, nk_read_cb cb, void* data);
int         nk_tcp_write(nk_tcp_t* tcp, const void* buf, int len, nk_write_cb cb, void* data);
void        nk_tcp_close(nk_tcp_t* tcp, nk_close_cb cb, void* data);
```

### UDP

```c
nk_udp_t*   nk_udp_new(nk_loop_t* loop);
int         nk_udp_bind(nk_udp_t* udp, const char* ip, int port);
int         nk_udp_recv(nk_udp_t* udp, nk_udp_recv_cb cb, void* data);
int         nk_udp_send(nk_udp_t* udp, const void* buf, int len, const char* ip, int port, nk_write_cb cb, void* data);
void        nk_udp_close(nk_udp_t* udp, nk_close_cb cb, void* data);
```

### Timer

```c
nk_timer_t* nk_timer_new(nk_loop_t* loop);
int         nk_timer_start(nk_timer_t* timer, uint64_t timeout_ms, uint64_t repeat_ms, nk_timer_cb cb, void* data);
void        nk_timer_stop(nk_timer_t* timer);
void        nk_timer_close(nk_timer_t* timer, nk_close_cb cb, void* data);
```

### Error

```c
const char* nk_strerror(int err);
```

### Callback Signatures

```c
typedef void (*nk_accept_cb)(nk_tcp_t* server, nk_tcp_t* client, int status, void* data);
typedef void (*nk_connect_cb)(nk_tcp_t* tcp, int status, void* data);
typedef void (*nk_read_cb)(nk_tcp_t* tcp, const void* buf, int nread, void* data);
typedef void (*nk_write_cb)(void* handle, int status, void* data);
typedef void (*nk_udp_recv_cb)(nk_udp_t* udp, const void* buf, int nread, const char* addr, int port, void* data);
typedef void (*nk_timer_cb)(nk_timer_t* timer, void* data);
typedef void (*nk_close_cb)(void* handle, void* data);
```

## Error Handling

- `NK_E_*` constants: `NK_E_NOMEM`, `NK_E_INVALID`, `NK_E_EOF`, `NK_E_TIMEOUT`, etc.
- Internal mapping from `WSAGetLastError()` to `NK_E_*`.
- `nk_strerror(int err)` returns human-readable string.
- `nread < 0` in read callbacks indicates error/EOF.

## Close / Lifecycle

1. User calls `nk_tcp_close(tcp, cb, data)`.
2. Handle sets `closing = 1`. New operations rejected with `NK_E_INVALID`.
3. Outstanding async ops complete (IOCP returns `ERROR_OPERATION_ABORTED` for cancelled ops).
4. Once all ops complete, user close callback fires, handle memory freed, `active_handles` decremented.
5. `nk_loop_run()` exits when `active_handles == 0` or `running == 0`.
6. `nk_loop_free()` force-closes all remaining handles (`CancelIoEx` + drain), then closes IOCP handle.

## File Structure

```
include/
  netkit.h                  umbrella header
  netkit/
    types.h                 forward declarations, callback typedefs
    err.h                   NK_E_* constants, nk_strerror declaration
    loop.h                  public loop API
    tcp.h                   public tcp API
    udp.h                   public udp API
    timer.h                 public timer API

src/
  loop.c / loop.h           loop implementation, IOCP main loop, timer dispatch
  handle.h                  nk_handle_t base, type enum
  op.c / op.h               nk_op_t, op type enum, alloc/free helpers
  tcp.c / tcp.h             tcp implementation
  udp.c / udp.h             udp implementation
  timer.c / timer.h         timer implementation + min-heap integration
  err.c / err.h             error codes, WSA mapping
  heap.c / heap.h           min-heap (for timers)
```

`include/netkit.h` includes: `netkit/types.h`, `netkit/err.h`, `netkit/loop.h`, `netkit/tcp.h`, `netkit/udp.h`, `netkit/timer.h`.

`src/*.h` are internal headers (not installed). `include/netkit/*.h` are public (opaque pointers + function declarations only).

## Testing

Uses project `tests/assert.h` (ASSERT macro). One test file per module, each compiled as an independent executable via `netkit_add_test()`.

| Test file | Scope |
|-----------|-------|
| test-heap | min-heap insert, delete, ordering (pure data structure) |
| test-err | error code mapping, nk_strerror return values |
| test-loop | create/destroy, empty loop exits immediately, stop behavior |
| test-timer | single-shot fires, repeat fires, stop cancels |
| test-tcp | loopback (127.0.0.1) accept -> connect -> write -> read -> close |
| test-udp | loopback send/recv |

All tests run on 127.0.0.1, no external network. Each test uses a timeout timer to prevent hangs.
