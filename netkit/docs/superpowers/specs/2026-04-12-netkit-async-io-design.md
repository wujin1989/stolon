# netkit Async I/O Library Design

## Summary

netkit is a Windows-only C library providing high-performance asynchronous I/O based on IOCP. It targets TCP, UDP, and timers with a single-threaded event loop model. It is a self-use infrastructure library.

## Architecture

Three-layer structure:

- **netkit_loop_t** — Event loop. Holds the IOCP handle, manages all handle lifetimes. Single-threaded, `netkit_loop_run()` blocks until no active handles remain or `netkit_loop_stop()` is called.
- **_handle_t** — Internal base type (not exposed to users). Contains type identifier, loop pointer, closing flag. Concrete types: `netkit_tcp_t`, `netkit_udp_t`, `netkit_timer_t`, each embedding `_handle_t` as the first field.
- **_op_t** — Internal async operation. Embeds `OVERLAPPED` as the first field (for pointer casting from IOCP completion). Contains operation type, callback pointer, `void* user_data`, and buffer info (`WSABUF` etc.). Created per read/write/connect/accept, freed after completion.

### Data Flow

1. User calls e.g. `netkit_tcp_read(tcp, cb, data)`
2. Library creates `_op_t`, submits `WSARecv` to IOCP
3. `netkit_loop_run()` calls `GetQueuedCompletionStatus()`
4. Casts `OVERLAPPED*` back to `_op_t*`
5. Dispatches by `op_type` to handler
6. Handler invokes user callback, frees op
7. Checks timer min-heap, fires expired timers

### Timer Implementation

Timers are managed via a min-heap sorted by expiration time. Each iteration of the loop uses the heap-top's remaining time as the `GetQueuedCompletionStatus` timeout parameter.

### TCP Accept

Uses `AcceptEx` for async accept. Pre-creates an accept socket stored in the op. On completion, calls `setsockopt(SO_UPDATE_ACCEPT_CONTEXT)` to inherit listener socket properties.

## Public API

Prefix: `netkit_`. All public functions return `int` (0 = success, -1 = error). Pointer-returning functions return NULL on failure.
Memory: library-allocated (`netkit_*_create()` / `netkit_*_destroy()` or async close).
Callback style: function pointer (`_fn_t` typedef) + `void* user_data`.

### Loop

```c
netkit_loop_t*  netkit_loop_create(void);
int             netkit_loop_run(netkit_loop_t* loop);
void            netkit_loop_stop(netkit_loop_t* loop);
void            netkit_loop_destroy(netkit_loop_t* loop);
```

### TCP

```c
netkit_tcp_t*   netkit_tcp_create(netkit_loop_t* loop);
int             netkit_tcp_bind(netkit_tcp_t* tcp, const char* ip, int32_t port);
int             netkit_tcp_listen(netkit_tcp_t* tcp, int32_t backlog, netkit_tcp_accept_fn_t cb, void* data);
int             netkit_tcp_connect(netkit_tcp_t* tcp, const char* ip, int32_t port, netkit_tcp_connect_fn_t cb, void* data);
int             netkit_tcp_read(netkit_tcp_t* tcp, netkit_tcp_read_fn_t cb, void* data);
int             netkit_tcp_write(netkit_tcp_t* tcp, const void* buf, int32_t len, netkit_tcp_write_fn_t cb, void* data);
void            netkit_tcp_close(netkit_tcp_t* tcp, netkit_close_fn_t cb, void* data);
```

### UDP

```c
netkit_udp_t*   netkit_udp_create(netkit_loop_t* loop);
int             netkit_udp_bind(netkit_udp_t* udp, const char* ip, int32_t port);
int             netkit_udp_recv(netkit_udp_t* udp, netkit_udp_recv_fn_t cb, void* data);
int             netkit_udp_send(netkit_udp_t* udp, const void* buf, int32_t len, const char* ip, int32_t port, netkit_udp_send_fn_t cb, void* data);
void            netkit_udp_close(netkit_udp_t* udp, netkit_close_fn_t cb, void* data);
```

### Timer

```c
netkit_timer_t* netkit_timer_create(netkit_loop_t* loop);
int             netkit_timer_start(netkit_timer_t* timer, uint64_t timeout_ms, uint64_t repeat_ms, netkit_timer_timeout_fn_t cb, void* data);
void            netkit_timer_stop(netkit_timer_t* timer);
void            netkit_timer_close(netkit_timer_t* timer, netkit_close_fn_t cb, void* data);
```

### Error

```c
const char*     netkit_err_strerror(int err);
```

### Callback Signatures

```c
typedef void (*netkit_tcp_accept_fn_t)(netkit_tcp_t* server, netkit_tcp_t* client, int status, void* data);
typedef void (*netkit_tcp_connect_fn_t)(netkit_tcp_t* tcp, int status, void* data);
typedef void (*netkit_tcp_read_fn_t)(netkit_tcp_t* tcp, const void* buf, int32_t nread, void* data);
typedef void (*netkit_tcp_write_fn_t)(netkit_tcp_t* tcp, int status, void* data);
typedef void (*netkit_udp_recv_fn_t)(netkit_udp_t* udp, const void* buf, int32_t nread, const char* addr, int32_t port, void* data);
typedef void (*netkit_udp_send_fn_t)(netkit_udp_t* udp, int status, void* data);
typedef void (*netkit_timer_timeout_fn_t)(netkit_timer_t* timer, void* data);
typedef void (*netkit_close_fn_t)(void* handle, void* data);
```

## Error Handling

- Error constants as enum `netkit_err_e` / `netkit_err_t` with values `NETKIT_ERR_NOMEM`, `NETKIT_ERR_INVALID`, `NETKIT_ERR_EOF`, `NETKIT_ERR_TIMEOUT`, etc.
- Internal mapping from `WSAGetLastError()` to `NETKIT_ERR_*`.
- `netkit_err_strerror(int err)` returns human-readable string.
- `nread < 0` in read callbacks indicates error/EOF.
- Functions return `0` on success, `-1` on failure. Pointer-returning functions return `NULL` on failure.

## Close / Lifecycle

1. User calls `netkit_tcp_close(tcp, cb, data)`.
2. Handle sets `closing = true`. New operations rejected (return `-1`).
3. Outstanding async ops complete (IOCP returns `ERROR_OPERATION_ABORTED` for cancelled ops).
4. Once all ops complete, user close callback fires, handle memory freed, `active_handles` decremented.
5. `netkit_loop_run()` exits when `active_handles == 0` or `running == false`.
6. `netkit_loop_destroy()` force-closes all remaining handles (`CancelIoEx` + drain), then closes IOCP handle.

Opaque types use `create`/`destroy` (library owns memory). Close is async due to pending IOCP operations.

## File Structure

```
include/
  netkit.h                              umbrella header
  netkit/
    netkit-types.h                      forward declarations, callback typedefs
    netkit-err.h                        NETKIT_ERR_* enum, netkit_err_strerror declaration
    netkit-loop.h                       public loop API
    netkit-tcp.h                        public tcp API
    netkit-udp.h                        public udp API
    netkit-timer.h                      public timer API

src/
  netkit-loop.c                         loop implementation, IOCP main loop, timer dispatch
  netkit-loop.h                         loop internal struct definition
  netkit-tcp.c                          tcp implementation
  netkit-tcp.h                          tcp internal struct definition
  netkit-udp.c                          udp implementation
  netkit-udp.h                          udp internal struct definition
  netkit-timer.c                        timer implementation + min-heap integration
  netkit-timer.h                        timer internal struct definition
  netkit-err.c                          error codes, WSA mapping
  netkit-err.h                          internal error helpers
  handle.h                              _handle_t base, handle type enum (internal)
  op.c                                  _op_t alloc/free helpers (internal)
  op.h                                  _op_t definition, op type enum (internal)
  heap.c                                min-heap implementation (internal)
  heap.h                                min-heap interface (internal)
```

`include/netkit.h` includes in order: `netkit/netkit-types.h`, `netkit/netkit-err.h`, `netkit/netkit-loop.h`, `netkit/netkit-tcp.h`, `netkit/netkit-udp.h`, `netkit/netkit-timer.h`.

Public headers contain only opaque forward declarations (`typedef struct netkit_tcp_s netkit_tcp_t;`) and `extern` function declarations with Doxygen comments. Internal headers in `src/` contain full struct definitions.

## Testing

Uses project `tests/assert.h` (ASSERT macro). One test file per module, each compiled as an independent executable via `netkit_add_test()`.

| Test file | Scope |
|-----------|-------|
| test-heap | min-heap insert, delete, ordering (pure data structure) |
| test-err | error code mapping, netkit_err_strerror return values |
| test-loop | create/destroy, empty loop exits immediately, stop behavior |
| test-timer | single-shot fires, repeat fires, stop cancels |
| test-tcp | loopback (127.0.0.1) accept -> connect -> write -> read -> close |
| test-udp | loopback send/recv |

All tests run on 127.0.0.1, no external network. Each test uses a timeout timer to prevent hangs.
