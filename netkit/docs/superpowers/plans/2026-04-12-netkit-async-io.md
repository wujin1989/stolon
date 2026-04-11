# netkit Async I/O Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement a Windows IOCP-based async I/O library with TCP, UDP, and timer support.

**Architecture:** Three-layer design (loop -> handle -> op) built on Windows IOCP. The event loop polls `GetQueuedCompletionStatus`, dispatches completed operations to callbacks, and manages timers via a min-heap. All types are opaque with `create`/`destroy` lifecycle.

**Tech Stack:** C11, Windows IOCP (`winsock2.h`, `mswsock.h`), CMake 3.25, MSVC

---

## Style Reference

All `.c` and `.h` files MUST follow `skills/c-project-style/references/style.md`. Key rules:

- License header: `/** Copyright ... */` block on every file (read from `LICENSE`)
- Header guards: `_Pragma("once")` (no `#ifndef`)
- Comments: `/* */` only (no `//`), ASCII only, Doxygen on all `extern` declarations
- Naming: `netkit_<module>_<action>` public, `_<module>_<action>` static, `_fn_t` callback typedefs
- Types: fixed-width integers for struct fields (`int32_t`, `uint32_t`, `uint64_t`), `bool` for flags
- Memory: `calloc` with explicit cast, NULL check, NULL-safe destroy
- Includes: grouped (own header, project headers, internal headers, stdlib)
- `extern` keyword on all function declarations in `.h` files

The license header for this project is:

```c
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
```

## File Map

| File | Responsibility | Created in |
|------|---------------|------------|
| `include/netkit/netkit-types.h` | Forward declarations, callback typedefs | Task 1 |
| `include/netkit/netkit-err.h` | Public error enum, `netkit_err_strerror` declaration | Task 2 |
| `src/netkit-err.c` | Error code definitions, WSA mapping, `netkit_err_strerror` | Task 2 |
| `src/netkit-err.h` | Internal error helpers | Task 2 |
| `src/heap.h` | Min-heap interface (for timers) | Task 3 |
| `src/heap.c` | Min-heap implementation | Task 3 |
| `src/handle.h` | `_handle_t` base struct, handle type enum | Task 4 |
| `src/op.h` | `_op_t` struct, op type enum | Task 4 |
| `src/op.c` | Op alloc/free helpers | Task 4 |
| `include/netkit/netkit-loop.h` | Public loop API declarations | Task 5 |
| `src/netkit-loop.h` | Internal loop struct definition | Task 5 |
| `src/netkit-loop.c` | Loop implementation (IOCP main loop, timer dispatch) | Task 5 |
| `include/netkit/netkit-timer.h` | Public timer API declarations | Task 6 |
| `src/netkit-timer.h` | Internal timer struct definition | Task 6 |
| `src/netkit-timer.c` | Timer implementation | Task 6 |
| `include/netkit/netkit-tcp.h` | Public TCP API declarations | Task 7 |
| `src/netkit-tcp.h` | Internal TCP struct definition | Task 7 |
| `src/netkit-tcp.c` | TCP implementation | Task 7 |
| `include/netkit/netkit-udp.h` | Public UDP API declarations | Task 8 |
| `src/netkit-udp.h` | Internal UDP struct definition | Task 8 |
| `src/netkit-udp.c` | UDP implementation | Task 8 |
| `include/netkit.h` | Umbrella header (modified) | Task 1 |
| `CMakeLists.txt` | Root build file (modified each task) | Tasks 2-8 |
| `tests/CMakeLists.txt` | Test registration (modified each task) | Tasks 2-8 |
| `tests/test-err.c` | Error module tests | Task 2 |
| `tests/test-heap.c` | Min-heap tests | Task 3 |
| `tests/test-loop.c` | Loop create/destroy/run/stop tests | Task 5 |
| `tests/test-timer.c` | Timer tests | Task 6 |
| `tests/test-tcp.c` | TCP loopback tests | Task 7 |
| `tests/test-udp.c` | UDP loopback tests | Task 8 |

---

### Task 1: Public Types and Umbrella Header

**Files:**
- Create: `include/netkit/netkit-types.h`
- Modify: `include/netkit.h`

- [ ] **Step 1: Create `include/netkit/netkit-types.h`**

This file contains all forward declarations and callback typedefs. No implementation.

```c
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

#include <stdint.h>

/* Opaque type forward declarations */
typedef struct netkit_loop_s  netkit_loop_t;
typedef struct netkit_tcp_s   netkit_tcp_t;
typedef struct netkit_udp_s   netkit_udp_t;
typedef struct netkit_timer_s netkit_timer_t;

/* Callback typedefs */
typedef void (*netkit_tcp_accept_fn_t)(
    netkit_tcp_t* server,
    netkit_tcp_t* client,
    int status,
    void* data);

typedef void (*netkit_tcp_connect_fn_t)(
    netkit_tcp_t* tcp,
    int status,
    void* data);

typedef void (*netkit_tcp_read_fn_t)(
    netkit_tcp_t* tcp,
    const void* buf,
    int32_t nread,
    void* data);

typedef void (*netkit_tcp_write_fn_t)(
    netkit_tcp_t* tcp,
    int status,
    void* data);

typedef void (*netkit_udp_recv_fn_t)(
    netkit_udp_t* udp,
    const void* buf,
    int32_t nread,
    const char* addr,
    int32_t port,
    void* data);

typedef void (*netkit_udp_send_fn_t)(
    netkit_udp_t* udp,
    int status,
    void* data);

typedef void (*netkit_timer_timeout_fn_t)(
    netkit_timer_t* timer,
    void* data);

typedef void (*netkit_close_fn_t)(
    void* handle,
    void* data);
```

- [ ] **Step 2: Update `include/netkit.h`**

Replace the placeholder comment with the module includes:

```c
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
#include "netkit/netkit-err.h"
#include "netkit/netkit-loop.h"
#include "netkit/netkit-tcp.h"
#include "netkit/netkit-udp.h"
#include "netkit/netkit-timer.h"
```

Note: the build will not succeed yet because the included headers do not exist. They are created in subsequent tasks. This is expected.

- [ ] **Step 3: Commit**

```bash
git add include/netkit/netkit-types.h include/netkit.h
git commit -m "feat: add public type forward declarations and callback typedefs"
```

---

### Task 2: Error Module

**Files:**
- Create: `include/netkit/netkit-err.h`
- Create: `src/netkit-err.h`
- Create: `src/netkit-err.c`
- Create: `tests/test-err.c`
- Modify: `CMakeLists.txt` (add `src/netkit-err.c` to SRCS)
- Modify: `tests/CMakeLists.txt` (add `netkit_add_test(err)`)

- [ ] **Step 1: Create `include/netkit/netkit-err.h`**

Public error enum and strerror declaration.

```c
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

/**
 * @brief Error codes returned by netkit functions.
 *
 * All values are negative. Zero means success.
 */
typedef enum netkit_err_e {
    NETKIT_ERR_NOMEM      = -1,  /**< Memory allocation failed. */
    NETKIT_ERR_INVALID    = -2,  /**< Invalid argument or state. */
    NETKIT_ERR_EOF        = -3,  /**< End of stream / connection closed. */
    NETKIT_ERR_TIMEOUT    = -4,  /**< Operation timed out. */
    NETKIT_ERR_CONNRESET  = -5,  /**< Connection reset by peer. */
    NETKIT_ERR_CONNREFUSED = -6, /**< Connection refused. */
    NETKIT_ERR_ADDRINUSE  = -7,  /**< Address already in use. */
    NETKIT_ERR_CANCELLED  = -8,  /**< Operation cancelled. */
    NETKIT_ERR_UNKNOWN    = -99  /**< Unmapped system error. */
} netkit_err_t;

/**
 * @brief Return a human-readable string for an error code.
 *
 * @param err  One of the NETKIT_ERR_* values.
 *
 * @return Static string describing the error. Never NULL.
 */
extern const char* netkit_err_strerror(int err);
```

- [ ] **Step 2: Create `src/netkit-err.h`**

Internal helper to map WSA errors.

```c
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

/**
 * @brief Map the current WSAGetLastError() value to a NETKIT_ERR_* code.
 *
 * @return One of the NETKIT_ERR_* values.
 */
extern int netkit_err_from_wsa(void);

/**
 * @brief Map a specific WSA error code to a NETKIT_ERR_* code.
 *
 * @param wsa_err  The WSA error code (e.g. WSAECONNRESET).
 *
 * @return One of the NETKIT_ERR_* values.
 */
extern int netkit_err_from_wsa_code(int wsa_err);
```

- [ ] **Step 3: Create `src/netkit-err.c`**

```c
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

#include "netkit/netkit-err.h"

#include "netkit-err.h"

#include <winsock2.h>

const char* netkit_err_strerror(int err) {
    switch (err) {
    case 0:
        return "success";
    case NETKIT_ERR_NOMEM:
        return "out of memory";
    case NETKIT_ERR_INVALID:
        return "invalid argument or state";
    case NETKIT_ERR_EOF:
        return "end of stream";
    case NETKIT_ERR_TIMEOUT:
        return "operation timed out";
    case NETKIT_ERR_CONNRESET:
        return "connection reset by peer";
    case NETKIT_ERR_CONNREFUSED:
        return "connection refused";
    case NETKIT_ERR_ADDRINUSE:
        return "address already in use";
    case NETKIT_ERR_CANCELLED:
        return "operation cancelled";
    case NETKIT_ERR_UNKNOWN:
        return "unknown error";
    default:
        return "unknown error";
    }
}

int netkit_err_from_wsa_code(int wsa_err) {
    switch (wsa_err) {
    case WSAECONNRESET:
        return NETKIT_ERR_CONNRESET;
    case WSAECONNREFUSED:
        return NETKIT_ERR_CONNREFUSED;
    case WSAEADDRINUSE:
        return NETKIT_ERR_ADDRINUSE;
    case WSAETIMEDOUT:
        return NETKIT_ERR_TIMEOUT;
    case WSA_OPERATION_ABORTED:
        return NETKIT_ERR_CANCELLED;
    case WSAESHUTDOWN:
        return NETKIT_ERR_EOF;
    default:
        return NETKIT_ERR_UNKNOWN;
    }
}

int netkit_err_from_wsa(void) {
    return netkit_err_from_wsa_code(WSAGetLastError());
}
```

- [ ] **Step 4: Write test `tests/test-err.c`**

```c
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
```

- [ ] **Step 5: Add `src/netkit-err.c` to `CMakeLists.txt` SRCS**

In the root `CMakeLists.txt`, change:

```cmake
set(SRCS
)
```

to:

```cmake
set(SRCS
    src/netkit-err.c
)
```

Also add `ws2_32` and `mswsock` link libraries. Change:

```cmake
target_link_libraries(netkit PRIVATE ${CMAKE_DL_LIBS})
```

to:

```cmake
target_link_libraries(netkit PRIVATE ws2_32 mswsock)
```

- [ ] **Step 6: Register test in `tests/CMakeLists.txt`**

Add after the `# netkit_add_test(<module>)` comment:

```cmake
netkit_add_test(err)
```

- [ ] **Step 7: Build and run test**

Run:
```bash
cmake -B out -DNETKIT_ENABLE_TESTING=ON
cmake --build out --config Debug
ctest --test-dir out -C Debug -R err --output-on-failure
```

Expected: test-err PASS

- [ ] **Step 8: Commit**

```bash
git add -A
git commit -m "feat: add error module with WSA mapping and strerror"
```

---

### Task 3: Min-Heap (Timer Infrastructure)

**Files:**
- Create: `src/heap.h`
- Create: `src/heap.c`
- Create: `tests/test-heap.c`
- Modify: `CMakeLists.txt` (add `src/heap.c` to SRCS)
- Modify: `tests/CMakeLists.txt` (add `netkit_add_test(heap)`)

- [ ] **Step 1: Create `src/heap.h`**

Internal min-heap for timer scheduling. Stores opaque `void*` entries sorted by a `uint64_t` key (expiration time). Supports insert, remove-min, peek-min, and remove-by-index.

```c
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

#include <stdint.h>
#include <stddef.h>

typedef struct _heap_entry_s {
    uint64_t key;   /**< Sort key (expiration time in ms). */
    void*    data;  /**< Opaque pointer to the timer handle. */
} _heap_entry_t;

typedef struct _heap_s {
    _heap_entry_t* entries;   /**< Dynamic array of entries. */
    size_t         size;      /**< Number of entries currently in the heap. */
    size_t         capacity;  /**< Allocated capacity of entries array. */
} _heap_t;

/**
 * @brief Initialize a heap. Caller owns the _heap_t memory.
 *
 * @param heap  Pointer to the heap to initialize.
 */
extern void _heap_init(_heap_t* heap);

/**
 * @brief Free internal allocations. Does not free the heap struct itself.
 *
 * @param heap  Pointer to the heap to clean up.
 */
extern void _heap_deinit(_heap_t* heap);

/**
 * @brief Insert an entry into the heap.
 *
 * @param heap  The heap.
 * @param key   Sort key (lower = higher priority).
 * @param data  Opaque data pointer.
 *
 * @return 0 on success, -1 on allocation failure.
 */
extern int _heap_insert(_heap_t* heap, uint64_t key, void* data);

/**
 * @brief Peek at the minimum entry without removing it.
 *
 * @param heap  The heap.
 * @param out   Output entry. Only valid if function returns 0.
 *
 * @return 0 if heap is non-empty, -1 if empty.
 */
extern int _heap_peek(_heap_t* heap, _heap_entry_t* out);

/**
 * @brief Remove and return the minimum entry.
 *
 * @param heap  The heap.
 * @param out   Output entry. Only valid if function returns 0.
 *
 * @return 0 if an entry was removed, -1 if heap was empty.
 */
extern int _heap_pop(_heap_t* heap, _heap_entry_t* out);

/**
 * @brief Remove the first entry whose data pointer matches.
 *
 * @param heap  The heap.
 * @param data  The data pointer to search for.
 *
 * @return 0 if found and removed, -1 if not found.
 */
extern int _heap_remove(_heap_t* heap, void* data);

/**
 * @brief Return the number of entries in the heap.
 *
 * @param heap  The heap.
 *
 * @return Number of entries.
 */
extern size_t _heap_size(_heap_t* heap);
```

- [ ] **Step 2: Create `src/heap.c`**

```c
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

#include "heap.h"

#include <stdlib.h>
#include <string.h>

#define _HEAP_INITIAL_CAP 16

/* Swap two heap entries. */
static void _heap_swap(_heap_entry_t* a, _heap_entry_t* b) {
    _heap_entry_t tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Bubble entry at idx up to restore heap property. */
static void _heap_sift_up(_heap_t* heap, size_t idx) {
    while (idx > 0) {
        size_t parent = (idx - 1) / 2;
        if (heap->entries[idx].key < heap->entries[parent].key) {
            _heap_swap(&heap->entries[idx], &heap->entries[parent]);
            idx = parent;
        } else {
            break;
        }
    }
}

/* Push entry at idx down to restore heap property. */
static void _heap_sift_down(_heap_t* heap, size_t idx) {
    for (;;) {
        size_t smallest = idx;
        size_t left     = 2 * idx + 1;
        size_t right    = 2 * idx + 2;

        if (left < heap->size &&
            heap->entries[left].key < heap->entries[smallest].key) {
            smallest = left;
        }
        if (right < heap->size &&
            heap->entries[right].key < heap->entries[smallest].key) {
            smallest = right;
        }
        if (smallest == idx) {
            break;
        }
        _heap_swap(&heap->entries[idx], &heap->entries[smallest]);
        idx = smallest;
    }
}

void _heap_init(_heap_t* heap) {
    heap->entries  = NULL;
    heap->size     = 0;
    heap->capacity = 0;
}

void _heap_deinit(_heap_t* heap) {
    free(heap->entries);
    heap->entries  = NULL;
    heap->size     = 0;
    heap->capacity = 0;
}

int _heap_insert(_heap_t* heap, uint64_t key, void* data) {
    if (heap->size == heap->capacity) {
        size_t new_cap = (heap->capacity == 0) ? _HEAP_INITIAL_CAP
                                               : heap->capacity * 2;
        _heap_entry_t* new_entries = (_heap_entry_t*)realloc(
            heap->entries,
            new_cap * sizeof(_heap_entry_t));
        if (!new_entries) {
            return -1;
        }
        heap->entries  = new_entries;
        heap->capacity = new_cap;
    }
    heap->entries[heap->size].key  = key;
    heap->entries[heap->size].data = data;
    heap->size++;
    _heap_sift_up(heap, heap->size - 1);
    return 0;
}

int _heap_peek(_heap_t* heap, _heap_entry_t* out) {
    if (heap->size == 0) {
        return -1;
    }
    *out = heap->entries[0];
    return 0;
}

int _heap_pop(_heap_t* heap, _heap_entry_t* out) {
    if (heap->size == 0) {
        return -1;
    }
    *out = heap->entries[0];
    heap->size--;
    if (heap->size > 0) {
        heap->entries[0] = heap->entries[heap->size];
        _heap_sift_down(heap, 0);
    }
    return 0;
}

int _heap_remove(_heap_t* heap, void* data) {
    size_t idx;
    for (idx = 0; idx < heap->size; idx++) {
        if (heap->entries[idx].data == data) {
            break;
        }
    }
    if (idx == heap->size) {
        return -1;
    }
    heap->size--;
    if (idx < heap->size) {
        heap->entries[idx] = heap->entries[heap->size];
        _heap_sift_down(heap, idx);
        _heap_sift_up(heap, idx);
    }
    return 0;
}

size_t _heap_size(_heap_t* heap) {
    return heap->size;
}
```

- [ ] **Step 3: Write test `tests/test-heap.c`**

```c
#include "netkit.h"
#include "assert.h"

#include "heap.h"

static void test_empty_heap(void) {
    _heap_t heap;
    _heap_init(&heap);
    ASSERT(_heap_size(&heap) == 0);

    _heap_entry_t entry;
    ASSERT(_heap_peek(&heap, &entry) == -1);
    ASSERT(_heap_pop(&heap, &entry) == -1);

    _heap_deinit(&heap);
}

static void test_insert_and_pop_single(void) {
    _heap_t heap;
    _heap_init(&heap);

    int dummy = 42;
    ASSERT(_heap_insert(&heap, 100, &dummy) == 0);
    ASSERT(_heap_size(&heap) == 1);

    _heap_entry_t entry;
    ASSERT(_heap_peek(&heap, &entry) == 0);
    ASSERT(entry.key == 100);
    ASSERT(entry.data == &dummy);

    ASSERT(_heap_pop(&heap, &entry) == 0);
    ASSERT(entry.key == 100);
    ASSERT(_heap_size(&heap) == 0);

    _heap_deinit(&heap);
}

static void test_min_ordering(void) {
    _heap_t heap;
    _heap_init(&heap);

    int a = 1, b = 2, c = 3;
    ASSERT(_heap_insert(&heap, 300, &c) == 0);
    ASSERT(_heap_insert(&heap, 100, &a) == 0);
    ASSERT(_heap_insert(&heap, 200, &b) == 0);

    _heap_entry_t entry;
    ASSERT(_heap_pop(&heap, &entry) == 0);
    ASSERT(entry.key == 100);
    ASSERT(entry.data == &a);

    ASSERT(_heap_pop(&heap, &entry) == 0);
    ASSERT(entry.key == 200);
    ASSERT(entry.data == &b);

    ASSERT(_heap_pop(&heap, &entry) == 0);
    ASSERT(entry.key == 300);
    ASSERT(entry.data == &c);

    ASSERT(_heap_size(&heap) == 0);
    _heap_deinit(&heap);
}

static void test_remove_by_data(void) {
    _heap_t heap;
    _heap_init(&heap);

    int a = 1, b = 2, c = 3;
    _heap_insert(&heap, 100, &a);
    _heap_insert(&heap, 200, &b);
    _heap_insert(&heap, 300, &c);

    ASSERT(_heap_remove(&heap, &b) == 0);
    ASSERT(_heap_size(&heap) == 2);

    _heap_entry_t entry;
    ASSERT(_heap_pop(&heap, &entry) == 0);
    ASSERT(entry.key == 100);

    ASSERT(_heap_pop(&heap, &entry) == 0);
    ASSERT(entry.key == 300);

    _heap_deinit(&heap);
}

static void test_remove_not_found(void) {
    _heap_t heap;
    _heap_init(&heap);

    int a = 1, b = 2;
    _heap_insert(&heap, 100, &a);
    ASSERT(_heap_remove(&heap, &b) == -1);
    ASSERT(_heap_size(&heap) == 1);

    _heap_deinit(&heap);
}

int main(void) {
    test_empty_heap();
    test_insert_and_pop_single();
    test_min_ordering();
    test_remove_by_data();
    test_remove_not_found();
    return 0;
}
```

- [ ] **Step 4: Add `src/heap.c` to `CMakeLists.txt` SRCS**

Change:

```cmake
set(SRCS
    src/netkit-err.c
)
```

to:

```cmake
set(SRCS
    src/netkit-err.c
    src/heap.c
)
```

- [ ] **Step 5: Register test in `tests/CMakeLists.txt`**

Add after `netkit_add_test(err)`:

```cmake
netkit_add_test(heap)
```

- [ ] **Step 6: Build and run test**

Run:
```bash
cmake -B out -DNETKIT_ENABLE_TESTING=ON
cmake --build out --config Debug
ctest --test-dir out -C Debug -R heap --output-on-failure
```

Expected: test-heap PASS

- [ ] **Step 7: Commit**

```bash
git add -A
git commit -m "feat: add min-heap data structure for timer scheduling"
```

---

### Task 4: Handle Base and Op Types

**Files:**
- Create: `src/handle.h`
- Create: `src/op.h`
- Create: `src/op.c`
- Modify: `CMakeLists.txt` (add `src/op.c` to SRCS)

These are internal types only -- no public headers, no tests of their own. They are tested indirectly through the loop, tcp, udp, and timer tests.

- [ ] **Step 1: Create `src/handle.h`**

Internal base type embedded as the first field in every concrete handle.

```c
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
    _handle_type_t   type;           /**< Handle type discriminator. */
    netkit_loop_t*   loop;           /**< Owning event loop. */
    bool             closing;        /**< True if close has been requested. */
    int32_t          pending_ops;    /**< Number of outstanding async ops. */
    netkit_close_fn_t close_cb;      /**< User close callback. */
    void*            close_data;     /**< User data for close callback. */
} _handle_t;
```

- [ ] **Step 2: Create `src/op.h`**

Internal async operation type. `OVERLAPPED` must be the first field.

```c
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

#include <winsock2.h>
#include <stdint.h>

#define OP_READ_BUF_SIZE 65536

typedef enum _op_type_e {
    _OP_ACCEPT  = 1,
    _OP_CONNECT = 2,
    _OP_READ    = 3,
    _OP_WRITE   = 4,
    _OP_RECV    = 5,
    _OP_SEND    = 6
} _op_type_t;

typedef struct _op_s {
    OVERLAPPED  overlapped;  /**< Must be first field for IOCP casting. */
    _op_type_t  type;        /**< Operation type discriminator. */
    void*       handle;      /**< Back-pointer to owning handle. */
    void*       cb;          /**< User callback (cast to specific fn type). */
    void*       user_data;   /**< User data for callback. */
    WSABUF      wsa_buf;     /**< Buffer descriptor for WSA operations. */
    uint8_t     read_buf[OP_READ_BUF_SIZE]; /**< Inline read buffer. */
    SOCKET      accept_sock; /**< Pre-created socket for AcceptEx. */
    uint8_t     addr_buf[2 * (sizeof(struct sockaddr_in6) + 16)]; /**< AcceptEx address buffer. */
} _op_t;

/**
 * @brief Allocate and zero-initialize an op.
 *
 * @param type    Operation type.
 * @param handle  Back-pointer to the owning handle.
 * @param cb      User callback function pointer.
 * @param data    User data for callback.
 *
 * @return Pointer to the new op, or NULL on allocation failure.
 */
extern _op_t* _op_create(_op_type_t type, void* handle, void* cb, void* data);

/**
 * @brief Free an op.
 *
 * @param op  The op to free. NULL is safe.
 */
extern void _op_destroy(_op_t* op);
```

- [ ] **Step 3: Create `src/op.c`**

```c
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

#include "op.h"

#include <stdlib.h>

_op_t* _op_create(_op_type_t type, void* handle, void* cb, void* data) {
    _op_t* op = (_op_t*)calloc(1, sizeof(_op_t));
    if (!op) {
        return NULL;
    }
    op->type        = type;
    op->handle      = handle;
    op->cb          = cb;
    op->user_data   = data;
    op->accept_sock = INVALID_SOCKET;
    return op;
}

void _op_destroy(_op_t* op) {
    if (!op) {
        return;
    }
    if (op->accept_sock != INVALID_SOCKET) {
        closesocket(op->accept_sock);
        op->accept_sock = INVALID_SOCKET;
    }
    free(op);
}
```

- [ ] **Step 4: Add `src/op.c` to `CMakeLists.txt` SRCS**

Change:

```cmake
set(SRCS
    src/netkit-err.c
    src/heap.c
)
```

to:

```cmake
set(SRCS
    src/netkit-err.c
    src/heap.c
    src/op.c
)
```

- [ ] **Step 5: Build to verify compilation**

Run:
```bash
cmake -B out -DNETKIT_ENABLE_TESTING=ON
cmake --build out --config Debug
```

Expected: build succeeds

- [ ] **Step 6: Commit**

```bash
git add -A
git commit -m "feat: add internal handle base type and async op type"
```

---

### Task 5: Event Loop

**Files:**
- Create: `include/netkit/netkit-loop.h`
- Create: `src/netkit-loop.h`
- Create: `src/netkit-loop.c`
- Create: `tests/test-loop.c`
- Modify: `CMakeLists.txt` (add `src/netkit-loop.c` to SRCS)
- Modify: `tests/CMakeLists.txt` (add `netkit_add_test(loop)`)

- [ ] **Step 1: Create `include/netkit/netkit-loop.h`**

Public loop API. Only opaque pointer and extern declarations.

```c
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

#include "netkit-types.h"

/**
 * @brief Create a new event loop.
 *
 * Initializes Winsock (WSAStartup) and creates an IOCP handle.
 *
 * @return New loop, or NULL on failure.
 */
extern netkit_loop_t* netkit_loop_create(void);

/**
 * @brief Run the event loop.
 *
 * Blocks until there are no active handles or netkit_loop_stop() is called.
 *
 * @param loop  The event loop.
 *
 * @return 0 on normal exit, -1 on error.
 */
extern int netkit_loop_run(netkit_loop_t* loop);

/**
 * @brief Signal the loop to stop after the current iteration.
 *
 * @param loop  The event loop.
 */
extern void netkit_loop_stop(netkit_loop_t* loop);

/**
 * @brief Destroy the event loop and free all resources.
 *
 * Force-closes any remaining handles. Calls WSACleanup.
 *
 * @param loop  The event loop. NULL is safe.
 */
extern void netkit_loop_destroy(netkit_loop_t* loop);
```

- [ ] **Step 2: Create `src/netkit-loop.h`**

Internal loop struct definition. Other modules (tcp, udp, timer) include this to access loop internals.

```c
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

#include "heap.h"

#include <winsock2.h>
#include <stdbool.h>
#include <stdint.h>

struct netkit_loop_s {
    HANDLE   iocp;            /**< IOCP completion port handle. */
    bool     running;         /**< False when stop has been requested. */
    int32_t  active_handles;  /**< Number of active (non-closing) handles. */
    _heap_t  timers;          /**< Min-heap of pending timers. */
};

/**
 * @brief Register a handle with the loop (increment active count).
 *
 * @param loop  The event loop.
 */
extern void _loop_handle_register(netkit_loop_t* loop);

/**
 * @brief Unregister a handle from the loop (decrement active count).
 *
 * @param loop  The event loop.
 */
extern void _loop_handle_unregister(netkit_loop_t* loop);

/**
 * @brief Associate a socket with the loop IOCP handle.
 *
 * @param loop  The event loop.
 * @param sock  The socket to associate.
 *
 * @return 0 on success, -1 on failure.
 */
extern int _loop_associate(netkit_loop_t* loop, SOCKET sock);
```

- [ ] **Step 3: Create `src/netkit-loop.c`**

```c
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

#include "netkit/netkit-loop.h"

#include "netkit-loop.h"
#include "netkit-timer.h"
#include "handle.h"
#include "op.h"

#include <winsock2.h>
#include <stdlib.h>

/* Get the IOCP timeout based on the nearest timer expiration. */
static DWORD _loop_calc_timeout(netkit_loop_t* loop) {
    _heap_entry_t entry;
    if (_heap_peek(&loop->timers, &entry) != 0) {
        return INFINITE;
    }
    uint64_t now = GetTickCount64();
    if (entry.key <= now) {
        return 0;
    }
    uint64_t diff = entry.key - now;
    if (diff > (uint64_t)0xFFFFFFFE) {
        return 0xFFFFFFFE;
    }
    return (DWORD)diff;
}

/* Fire all expired timers. */
static void _loop_process_timers(netkit_loop_t* loop) {
    uint64_t now = GetTickCount64();
    for (;;) {
        _heap_entry_t entry;
        if (_heap_peek(&loop->timers, &entry) != 0) {
            break;
        }
        if (entry.key > now) {
            break;
        }
        _heap_pop(&loop->timers, &entry);
        _timer_fire((netkit_timer_t*)entry.data);
    }
}

/* Process a single IOCP completion. */
static void _loop_dispatch(
    _op_t* op,
    DWORD bytes_transferred,
    BOOL success) {

    _handle_t* h = (_handle_t*)op->handle;
    h->pending_ops--;

    switch (op->type) {
    case _OP_ACCEPT:
        _tcp_on_accept(op, bytes_transferred, success);
        break;
    case _OP_CONNECT:
        _tcp_on_connect(op, success);
        break;
    case _OP_READ:
        _tcp_on_read(op, bytes_transferred, success);
        break;
    case _OP_WRITE:
        _tcp_on_write(op, success);
        break;
    case _OP_RECV:
        _udp_on_recv(op, bytes_transferred, success);
        break;
    case _OP_SEND:
        _udp_on_send(op, success);
        break;
    }

    /* If handle is closing and no more pending ops, finish the close. */
    if (h->closing && h->pending_ops == 0) {
        if (h->close_cb) {
            h->close_cb(op->handle, h->close_data);
        }
        _loop_handle_unregister(h->loop);
        free(op->handle);
    }

    _op_destroy(op);
}

netkit_loop_t* netkit_loop_create(void) {
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        return NULL;
    }

    netkit_loop_t* loop = (netkit_loop_t*)calloc(1, sizeof(netkit_loop_t));
    if (!loop) {
        WSACleanup();
        return NULL;
    }

    loop->iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
    if (!loop->iocp) {
        free(loop);
        WSACleanup();
        return NULL;
    }

    loop->running        = true;
    loop->active_handles = 0;
    _heap_init(&loop->timers);

    return loop;
}

int netkit_loop_run(netkit_loop_t* loop) {
    if (!loop) {
        return -1;
    }

    while (loop->running && loop->active_handles > 0) {
        DWORD timeout = _loop_calc_timeout(loop);
        DWORD bytes   = 0;
        ULONG_PTR key = 0;
        OVERLAPPED* ov = NULL;

        BOOL ok = GetQueuedCompletionStatus(
            loop->iocp,
            &bytes,
            &key,
            &ov,
            timeout);

        /* Process expired timers regardless of IOCP result. */
        _loop_process_timers(loop);

        if (ov) {
            _op_t* op = (_op_t*)ov;
            _loop_dispatch(op, bytes, ok);
        }
    }

    return 0;
}

void netkit_loop_stop(netkit_loop_t* loop) {
    if (!loop) {
        return;
    }
    loop->running = false;
    /* Post a dummy completion to wake up GetQueuedCompletionStatus. */
    PostQueuedCompletionStatus(loop->iocp, 0, 0, NULL);
}

void netkit_loop_destroy(netkit_loop_t* loop) {
    if (!loop) {
        return;
    }
    if (loop->iocp) {
        CloseHandle(loop->iocp);
    }
    _heap_deinit(&loop->timers);
    free(loop);
    WSACleanup();
}

void _loop_handle_register(netkit_loop_t* loop) {
    loop->active_handles++;
}

void _loop_handle_unregister(netkit_loop_t* loop) {
    loop->active_handles--;
}

int _loop_associate(netkit_loop_t* loop, SOCKET sock) {
    HANDLE result = CreateIoCompletionPort(
        (HANDLE)sock,
        loop->iocp,
        0,
        0);
    return result ? 0 : -1;
}
```

Note: This file references `_tcp_on_accept`, `_tcp_on_connect`, `_tcp_on_read`, `_tcp_on_write`, `_udp_on_recv`, `_udp_on_send`, and `_timer_fire` which are defined in Tasks 6, 7, and 8. For the build to succeed at this stage, we need forward declarations. Add them to the internal headers in those tasks. For now, to make the loop compile standalone, add a temporary forward-declaration header `src/netkit-tcp.h` and `src/netkit-udp.h` and `src/netkit-timer.h` with stub declarations. These will be replaced with full implementations in later tasks.

Create minimal `src/netkit-tcp.h`:

```c
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

#include "op.h"

#include <windows.h>

/**
 * @brief Handle AcceptEx completion.
 */
extern void _tcp_on_accept(_op_t* op, DWORD bytes, BOOL success);

/**
 * @brief Handle ConnectEx completion.
 */
extern void _tcp_on_connect(_op_t* op, BOOL success);

/**
 * @brief Handle WSARecv completion.
 */
extern void _tcp_on_read(_op_t* op, DWORD bytes, BOOL success);

/**
 * @brief Handle WSASend completion.
 */
extern void _tcp_on_write(_op_t* op, BOOL success);
```

Create minimal `src/netkit-udp.h`:

```c
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

#include "op.h"

#include <windows.h>

/**
 * @brief Handle WSARecvFrom completion.
 */
extern void _udp_on_recv(_op_t* op, DWORD bytes, BOOL success);

/**
 * @brief Handle WSASendTo completion.
 */
extern void _udp_on_send(_op_t* op, BOOL success);
```

Create minimal `src/netkit-timer.h`:

```c
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

/**
 * @brief Fire a timer that has expired. Called by the loop.
 *
 * @param timer  The timer handle.
 */
extern void _timer_fire(netkit_timer_t* timer);
```

- [ ] **Step 4: Create stub implementations for forward declarations**

Create `src/netkit-tcp.c` (stub -- replaced in Task 7):

```c
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

#include "netkit-tcp.h"

void _tcp_on_accept(_op_t* op, DWORD bytes, BOOL success) {
    (void)op;
    (void)bytes;
    (void)success;
}

void _tcp_on_connect(_op_t* op, BOOL success) {
    (void)op;
    (void)success;
}

void _tcp_on_read(_op_t* op, DWORD bytes, BOOL success) {
    (void)op;
    (void)bytes;
    (void)success;
}

void _tcp_on_write(_op_t* op, BOOL success) {
    (void)op;
    (void)success;
}
```

Create `src/netkit-udp.c` (stub -- replaced in Task 8):

```c
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

#include "netkit-udp.h"

void _udp_on_recv(_op_t* op, DWORD bytes, BOOL success) {
    (void)op;
    (void)bytes;
    (void)success;
}

void _udp_on_send(_op_t* op, BOOL success) {
    (void)op;
    (void)success;
}
```

Create `src/netkit-timer.c` (stub -- replaced in Task 6):

```c
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

#include "netkit-timer.h"

#include "handle.h"

struct netkit_timer_s {
    _handle_t base;
};

void _timer_fire(netkit_timer_t* timer) {
    (void)timer;
}
```

- [ ] **Step 5: Update `CMakeLists.txt` SRCS**

Change:

```cmake
set(SRCS
    src/netkit-err.c
    src/heap.c
    src/op.c
)
```

to:

```cmake
set(SRCS
    src/netkit-err.c
    src/heap.c
    src/op.c
    src/netkit-loop.c
    src/netkit-tcp.c
    src/netkit-udp.c
    src/netkit-timer.c
)
```

- [ ] **Step 6: Write test `tests/test-loop.c`**

```c
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
```

- [ ] **Step 7: Register test in `tests/CMakeLists.txt`**

Add after `netkit_add_test(heap)`:

```cmake
netkit_add_test(loop)
```

- [ ] **Step 8: Build and run tests**

Run:
```bash
cmake -B out -DNETKIT_ENABLE_TESTING=ON
cmake --build out --config Debug
ctest --test-dir out -C Debug --output-on-failure
```

Expected: all tests PASS (err, heap, loop)

- [ ] **Step 9: Commit**

```bash
git add -A
git commit -m "feat: add event loop with IOCP and timer heap integration"
```

---

### Task 6: Timer Module

**Files:**
- Create: `include/netkit/netkit-timer.h`
- Modify: `src/netkit-timer.h` (replace stub with full internal struct + `_timer_fire` declaration)
- Modify: `src/netkit-timer.c` (replace stub with full implementation)
- Create: `tests/test-timer.c`
- Modify: `tests/CMakeLists.txt` (add `netkit_add_test(timer)`)

- [ ] **Step 1: Create `include/netkit/netkit-timer.h`**

```c
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

#include "netkit-types.h"

#include <stdint.h>

/**
 * @brief Create a new timer handle.
 *
 * @param loop  The event loop to attach to.
 *
 * @return New timer, or NULL on failure.
 */
extern netkit_timer_t* netkit_timer_create(netkit_loop_t* loop);

/**
 * @brief Start the timer.
 *
 * @param timer       The timer handle.
 * @param timeout_ms  Initial delay in milliseconds.
 * @param repeat_ms   Repeat interval in ms. 0 for one-shot.
 * @param cb          Callback fired on expiration.
 * @param data        User data passed to callback.
 *
 * @return 0 on success, -1 on failure.
 */
extern int netkit_timer_start(
    netkit_timer_t* timer,
    uint64_t timeout_ms,
    uint64_t repeat_ms,
    netkit_timer_timeout_fn_t cb,
    void* data);

/**
 * @brief Stop the timer. It will not fire again until restarted.
 *
 * @param timer  The timer handle.
 */
extern void netkit_timer_stop(netkit_timer_t* timer);

/**
 * @brief Close the timer and release resources.
 *
 * The close callback fires after the timer is fully cleaned up.
 *
 * @param timer  The timer handle.
 * @param cb     Close callback (may be NULL).
 * @param data   User data for close callback.
 */
extern void netkit_timer_close(
    netkit_timer_t* timer,
    netkit_close_fn_t cb,
    void* data);
```

- [ ] **Step 2: Replace `src/netkit-timer.h`**

Replace the entire stub file with:

```c
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
```

- [ ] **Step 3: Replace `src/netkit-timer.c`**

Replace the entire stub file with:

```c
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
```

- [ ] **Step 4: Write test `tests/test-timer.c`**

```c
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
    (void)data;
    /* Safety timer: if we get here, the test is hanging. Force stop. */
    netkit_loop_t* loop = *(netkit_loop_t**)data;
    netkit_timer_close(timer, NULL, NULL);
    netkit_loop_stop(loop);
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
```

- [ ] **Step 5: Register test in `tests/CMakeLists.txt`**

Add after `netkit_add_test(loop)`:

```cmake
netkit_add_test(timer)
```

- [ ] **Step 6: Build and run tests**

Run:
```bash
cmake -B out -DNETKIT_ENABLE_TESTING=ON
cmake --build out --config Debug
ctest --test-dir out -C Debug --output-on-failure
```

Expected: all tests PASS (err, heap, loop, timer)

- [ ] **Step 7: Commit**

```bash
git add -A
git commit -m "feat: add timer module with single-shot and repeat support"
```

---

### Task 7: TCP Module

**Files:**
- Create: `include/netkit/netkit-tcp.h`
- Modify: `src/netkit-tcp.h` (replace stub with full internal struct)
- Modify: `src/netkit-tcp.c` (replace stub with full implementation)
- Create: `tests/test-tcp.c`
- Modify: `tests/CMakeLists.txt` (add `netkit_add_test(tcp)`)

- [ ] **Step 1: Create `include/netkit/netkit-tcp.h`**

```c
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

#include "netkit-types.h"

#include <stdint.h>

/**
 * @brief Create a new TCP handle.
 *
 * @param loop  The event loop.
 *
 * @return New TCP handle, or NULL on failure.
 */
extern netkit_tcp_t* netkit_tcp_create(netkit_loop_t* loop);

/**
 * @brief Bind the TCP socket to a local address.
 *
 * @param tcp   The TCP handle.
 * @param ip    IP address string (e.g. "127.0.0.1").
 * @param port  Port number.
 *
 * @return 0 on success, -1 on failure.
 */
extern int netkit_tcp_bind(netkit_tcp_t* tcp, const char* ip, int32_t port);

/**
 * @brief Start listening for incoming connections.
 *
 * @param tcp      The TCP handle (must be bound).
 * @param backlog  Listen backlog size.
 * @param cb       Callback invoked for each accepted connection.
 * @param data     User data for callback.
 *
 * @return 0 on success, -1 on failure.
 */
extern int netkit_tcp_listen(
    netkit_tcp_t* tcp,
    int32_t backlog,
    netkit_tcp_accept_fn_t cb,
    void* data);

/**
 * @brief Initiate an async connection to a remote address.
 *
 * @param tcp   The TCP handle.
 * @param ip    Remote IP address.
 * @param port  Remote port.
 * @param cb    Callback invoked when connection completes.
 * @param data  User data for callback.
 *
 * @return 0 on success, -1 on failure.
 */
extern int netkit_tcp_connect(
    netkit_tcp_t* tcp,
    const char* ip,
    int32_t port,
    netkit_tcp_connect_fn_t cb,
    void* data);

/**
 * @brief Start reading data from the TCP connection.
 *
 * @param tcp   The TCP handle.
 * @param cb    Callback invoked when data arrives. nread < 0 means error/EOF.
 * @param data  User data for callback.
 *
 * @return 0 on success, -1 on failure.
 */
extern int netkit_tcp_read(
    netkit_tcp_t* tcp,
    netkit_tcp_read_fn_t cb,
    void* data);

/**
 * @brief Write data to the TCP connection.
 *
 * @param tcp   The TCP handle.
 * @param buf   Data buffer.
 * @param len   Number of bytes to write.
 * @param cb    Callback invoked when write completes.
 * @param data  User data for callback.
 *
 * @return 0 on success, -1 on failure.
 */
extern int netkit_tcp_write(
    netkit_tcp_t* tcp,
    const void* buf,
    int32_t len,
    netkit_tcp_write_fn_t cb,
    void* data);

/**
 * @brief Close the TCP handle asynchronously.
 *
 * Pending operations complete with NETKIT_ERR_CANCELLED. The close callback
 * fires after all operations finish and the socket is closed.
 *
 * @param tcp   The TCP handle.
 * @param cb    Close callback (may be NULL).
 * @param data  User data for close callback.
 */
extern void netkit_tcp_close(
    netkit_tcp_t* tcp,
    netkit_close_fn_t cb,
    void* data);
```

- [ ] **Step 2: Replace `src/netkit-tcp.h`**

Replace the entire file with the full internal struct and IOCP completion handler declarations:

```c
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
#include "op.h"

#include <winsock2.h>
#include <windows.h>

struct netkit_tcp_s {
    _handle_t              base;         /**< Embedded handle base. */
    SOCKET                 sock;         /**< Winsock socket. */
    netkit_tcp_accept_fn_t accept_cb;    /**< Accept callback (server). */
    void*                  accept_data;  /**< Accept callback user data. */
};

/**
 * @brief Handle AcceptEx completion.
 */
extern void _tcp_on_accept(_op_t* op, DWORD bytes, BOOL success);

/**
 * @brief Handle ConnectEx completion.
 */
extern void _tcp_on_connect(_op_t* op, BOOL success);

/**
 * @brief Handle WSARecv completion.
 */
extern void _tcp_on_read(_op_t* op, DWORD bytes, BOOL success);

/**
 * @brief Handle WSASend completion.
 */
extern void _tcp_on_write(_op_t* op, BOOL success);
```

- [ ] **Step 3: Replace `src/netkit-tcp.c`**

Replace the entire stub file with the full TCP implementation. This is the largest file in the project. Key points:

- `netkit_tcp_create` creates a socket with `WSASocketW` (overlapped flag), associates with IOCP.
- `netkit_tcp_listen` calls `listen()` then posts an initial `AcceptEx`.
- `_tcp_on_accept` completes the accept, creates a new `netkit_tcp_t` for the client, calls user callback, then posts another `AcceptEx` for the next connection.
- `netkit_tcp_connect` uses `ConnectEx` (loaded via `WSAIoctl`).
- `netkit_tcp_read` posts `WSARecv`.
- `netkit_tcp_write` posts `WSASend`.
- `netkit_tcp_close` sets closing flag, calls `CancelIoEx` to cancel pending ops.

```c
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

#include "netkit/netkit-tcp.h"
#include "netkit/netkit-err.h"

#include "netkit-tcp.h"
#include "netkit-loop.h"
#include "netkit-err.h"

#include <mswsock.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <string.h>

/* Load ConnectEx function pointer via WSAIoctl. */
static LPFN_CONNECTEX _tcp_get_connectex(SOCKET sock) {
    LPFN_CONNECTEX fn = NULL;
    GUID guid = WSAID_CONNECTEX;
    DWORD bytes = 0;
    WSAIoctl(
        sock,
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &guid,
        sizeof(guid),
        &fn,
        sizeof(fn),
        &bytes,
        NULL,
        NULL);
    return fn;
}

/* Load AcceptEx function pointer via WSAIoctl. */
static LPFN_ACCEPTEX _tcp_get_acceptex(SOCKET sock) {
    LPFN_ACCEPTEX fn = NULL;
    GUID guid = WSAID_ACCEPTEX;
    DWORD bytes = 0;
    WSAIoctl(
        sock,
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &guid,
        sizeof(guid),
        &fn,
        sizeof(fn),
        &bytes,
        NULL,
        NULL);
    return fn;
}

/* Post a new AcceptEx operation for the server socket. */
static int _tcp_post_accept(netkit_tcp_t* tcp) {
    _op_t* op = _op_create(_OP_ACCEPT, tcp, (void*)tcp->accept_cb, tcp->accept_data);
    if (!op) {
        return -1;
    }

    op->accept_sock = WSASocketW(
        AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (op->accept_sock == INVALID_SOCKET) {
        _op_destroy(op);
        return -1;
    }

    LPFN_ACCEPTEX accept_ex = _tcp_get_acceptex(tcp->sock);
    if (!accept_ex) {
        _op_destroy(op);
        return -1;
    }

    DWORD bytes = 0;
    BOOL ok = accept_ex(
        tcp->sock,
        op->accept_sock,
        op->addr_buf,
        0,
        sizeof(struct sockaddr_in6) + 16,
        sizeof(struct sockaddr_in6) + 16,
        &bytes,
        &op->overlapped);

    if (!ok && WSAGetLastError() != ERROR_IO_PENDING) {
        _op_destroy(op);
        return -1;
    }

    tcp->base.pending_ops++;
    return 0;
}

netkit_tcp_t* netkit_tcp_create(netkit_loop_t* loop) {
    if (!loop) {
        return NULL;
    }

    SOCKET sock = WSASocketW(
        AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (sock == INVALID_SOCKET) {
        return NULL;
    }

    netkit_tcp_t* tcp = (netkit_tcp_t*)calloc(1, sizeof(netkit_tcp_t));
    if (!tcp) {
        closesocket(sock);
        return NULL;
    }

    tcp->base.type = _HANDLE_TCP;
    tcp->base.loop = loop;
    tcp->sock      = sock;

    if (_loop_associate(loop, sock) != 0) {
        closesocket(sock);
        free(tcp);
        return NULL;
    }

    _loop_handle_register(loop);
    return tcp;
}

int netkit_tcp_bind(netkit_tcp_t* tcp, const char* ip, int32_t port) {
    if (!tcp || !ip || tcp->base.closing) {
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons((uint16_t)port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1) {
        return -1;
    }

    if (bind(tcp->sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        return -1;
    }
    return 0;
}

int netkit_tcp_listen(
    netkit_tcp_t* tcp,
    int32_t backlog,
    netkit_tcp_accept_fn_t cb,
    void* data) {

    if (!tcp || !cb || tcp->base.closing) {
        return -1;
    }

    if (listen(tcp->sock, backlog) == SOCKET_ERROR) {
        return -1;
    }

    tcp->accept_cb   = cb;
    tcp->accept_data = data;

    return _tcp_post_accept(tcp);
}

int netkit_tcp_connect(
    netkit_tcp_t* tcp,
    const char* ip,
    int32_t port,
    netkit_tcp_connect_fn_t cb,
    void* data) {

    if (!tcp || !ip || !cb || tcp->base.closing) {
        return -1;
    }

    /* ConnectEx requires the socket to be bound first. */
    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family      = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port        = 0;
    if (bind(tcp->sock, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR) {
        return -1;
    }

    struct sockaddr_in remote;
    memset(&remote, 0, sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_port   = htons((uint16_t)port);
    if (inet_pton(AF_INET, ip, &remote.sin_addr) != 1) {
        return -1;
    }

    LPFN_CONNECTEX connect_ex = _tcp_get_connectex(tcp->sock);
    if (!connect_ex) {
        return -1;
    }

    _op_t* op = _op_create(_OP_CONNECT, tcp, (void*)cb, data);
    if (!op) {
        return -1;
    }

    BOOL ok = connect_ex(
        tcp->sock,
        (struct sockaddr*)&remote,
        sizeof(remote),
        NULL,
        0,
        NULL,
        &op->overlapped);

    if (!ok && WSAGetLastError() != ERROR_IO_PENDING) {
        _op_destroy(op);
        return -1;
    }

    tcp->base.pending_ops++;
    return 0;
}

int netkit_tcp_read(
    netkit_tcp_t* tcp,
    netkit_tcp_read_fn_t cb,
    void* data) {

    if (!tcp || !cb || tcp->base.closing) {
        return -1;
    }

    _op_t* op = _op_create(_OP_READ, tcp, (void*)cb, data);
    if (!op) {
        return -1;
    }

    op->wsa_buf.buf = (char*)op->read_buf;
    op->wsa_buf.len = OP_READ_BUF_SIZE;

    DWORD flags = 0;
    int rc = WSARecv(
        tcp->sock,
        &op->wsa_buf,
        1,
        NULL,
        &flags,
        &op->overlapped,
        NULL);

    if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        _op_destroy(op);
        return -1;
    }

    tcp->base.pending_ops++;
    return 0;
}

int netkit_tcp_write(
    netkit_tcp_t* tcp,
    const void* buf,
    int32_t len,
    netkit_tcp_write_fn_t cb,
    void* data) {

    if (!tcp || !buf || len <= 0 || tcp->base.closing) {
        return -1;
    }

    _op_t* op = _op_create(_OP_WRITE, tcp, (void*)cb, data);
    if (!op) {
        return -1;
    }

    op->wsa_buf.buf = (char*)buf;
    op->wsa_buf.len = (ULONG)len;

    int rc = WSASend(
        tcp->sock,
        &op->wsa_buf,
        1,
        NULL,
        0,
        &op->overlapped,
        NULL);

    if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        _op_destroy(op);
        return -1;
    }

    tcp->base.pending_ops++;
    return 0;
}

void netkit_tcp_close(
    netkit_tcp_t* tcp,
    netkit_close_fn_t cb,
    void* data) {

    if (!tcp || tcp->base.closing) {
        return;
    }

    tcp->base.closing    = true;
    tcp->base.close_cb   = cb;
    tcp->base.close_data = data;

    /* Cancel all pending I/O on this socket. */
    CancelIoEx((HANDLE)tcp->sock, NULL);
    closesocket(tcp->sock);
    tcp->sock = INVALID_SOCKET;

    /* If no pending ops, close immediately. */
    if (tcp->base.pending_ops == 0) {
        if (cb) {
            cb(tcp, data);
        }
        _loop_handle_unregister(tcp->base.loop);
        free(tcp);
    }
}

void _tcp_on_accept(_op_t* op, DWORD bytes, BOOL success) {
    (void)bytes;
    netkit_tcp_t* server = (netkit_tcp_t*)op->handle;
    netkit_tcp_accept_fn_t cb = (netkit_tcp_accept_fn_t)op->cb;

    if (!success || server->base.closing) {
        if (op->accept_sock != INVALID_SOCKET) {
            closesocket(op->accept_sock);
            op->accept_sock = INVALID_SOCKET;
        }
        if (cb && !server->base.closing) {
            cb(server, NULL, -1, op->user_data);
        }
        return;
    }

    /* Inherit listen socket properties. */
    setsockopt(
        op->accept_sock,
        SOL_SOCKET,
        SO_UPDATE_ACCEPT_CONTEXT,
        (char*)&server->sock,
        sizeof(server->sock));

    /* Create a new tcp handle for the accepted client. */
    netkit_tcp_t* client = (netkit_tcp_t*)calloc(1, sizeof(netkit_tcp_t));
    if (!client) {
        closesocket(op->accept_sock);
        op->accept_sock = INVALID_SOCKET;
        if (cb) {
            cb(server, NULL, -1, op->user_data);
        }
    } else {
        client->base.type = _HANDLE_TCP;
        client->base.loop = server->base.loop;
        client->sock      = op->accept_sock;
        op->accept_sock   = INVALID_SOCKET;

        _loop_associate(server->base.loop, client->sock);
        _loop_handle_register(server->base.loop);

        if (cb) {
            cb(server, client, 0, op->user_data);
        }
    }

    /* Post next accept if server is still listening. */
    if (!server->base.closing) {
        _tcp_post_accept(server);
    }
}

void _tcp_on_connect(_op_t* op, BOOL success) {
    netkit_tcp_t* tcp = (netkit_tcp_t*)op->handle;
    netkit_tcp_connect_fn_t cb = (netkit_tcp_connect_fn_t)op->cb;

    if (success) {
        /* Enable normal send/recv after ConnectEx. */
        setsockopt(tcp->sock, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0);
    }

    if (cb) {
        cb(tcp, success ? 0 : -1, op->user_data);
    }
}

void _tcp_on_read(_op_t* op, DWORD bytes, BOOL success) {
    netkit_tcp_t* tcp = (netkit_tcp_t*)op->handle;
    netkit_tcp_read_fn_t cb = (netkit_tcp_read_fn_t)op->cb;

    if (!success || bytes == 0) {
        if (cb) {
            int32_t err = (bytes == 0) ? NETKIT_ERR_EOF
                                       : netkit_err_from_wsa();
            cb(tcp, NULL, err, op->user_data);
        }
        return;
    }

    if (cb) {
        cb(tcp, op->read_buf, (int32_t)bytes, op->user_data);
    }
}

void _tcp_on_write(_op_t* op, BOOL success) {
    netkit_tcp_t* tcp = (netkit_tcp_t*)op->handle;
    netkit_tcp_write_fn_t cb = (netkit_tcp_write_fn_t)op->cb;

    if (cb) {
        cb(tcp, success ? 0 : -1, op->user_data);
    }
}
```

- [ ] **Step 4: Write test `tests/test-tcp.c`**

Full loopback test: server listens, client connects, client writes, server reads, both close.

```c
#include "netkit.h"
#include "assert.h"

#include <string.h>

#define TEST_PORT 19876
#define TEST_MSG  "hello netkit"

static netkit_loop_t* _loop = NULL;

static void _on_safety_timeout(netkit_timer_t* timer, void* data) {
    (void)data;
    netkit_timer_close(timer, NULL, NULL);
    netkit_loop_stop(_loop);
    ASSERT(0);
}

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
    netkit_tcp_close(tcp, _on_client_close, NULL);
}

static void _on_write_done(netkit_tcp_t* tcp, int status, void* data) {
    (void)data;
    ASSERT(status == 0);
    netkit_tcp_close(tcp, _on_client_close, NULL);
}

static void _on_connect(netkit_tcp_t* tcp, int status, void* data) {
    (void)data;
    ASSERT(status == 0);
    netkit_tcp_write(tcp, TEST_MSG, (int32_t)strlen(TEST_MSG), _on_write_done, NULL);
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

static void test_tcp_echo(void) {
    _loop = netkit_loop_create();
    ASSERT(_loop != NULL);

    /* Safety timer. */
    netkit_timer_t* safety = netkit_timer_create(_loop);
    ASSERT(safety != NULL);
    netkit_timer_start(safety, 5000, 0, _on_safety_timeout, NULL);

    /* Server. */
    netkit_tcp_t* server = netkit_tcp_create(_loop);
    ASSERT(server != NULL);
    ASSERT(netkit_tcp_bind(server, "127.0.0.1", TEST_PORT) == 0);
    ASSERT(netkit_tcp_listen(server, 1, _on_accept, NULL) == 0);

    /* Client. */
    netkit_tcp_t* client = netkit_tcp_create(_loop);
    ASSERT(client != NULL);
    ASSERT(netkit_tcp_connect(client, "127.0.0.1", TEST_PORT, _on_connect, NULL) == 0);

    netkit_loop_run(_loop);

    /* Clean up safety timer if still alive. */
    netkit_loop_destroy(_loop);
}

int main(void) {
    test_tcp_echo();
    return 0;
}
```

- [ ] **Step 5: Register test in `tests/CMakeLists.txt`**

Add after `netkit_add_test(timer)`:

```cmake
netkit_add_test(tcp)
```

- [ ] **Step 6: Build and run tests**

Run:
```bash
cmake -B out -DNETKIT_ENABLE_TESTING=ON
cmake --build out --config Debug
ctest --test-dir out -C Debug --output-on-failure
```

Expected: all tests PASS (err, heap, loop, timer, tcp)

- [ ] **Step 7: Commit**

```bash
git add -A
git commit -m "feat: add TCP module with IOCP-based accept, connect, read, write"
```

---

### Task 8: UDP Module

**Files:**
- Create: `include/netkit/netkit-udp.h`
- Modify: `src/netkit-udp.h` (replace stub with full internal struct)
- Modify: `src/netkit-udp.c` (replace stub with full implementation)
- Create: `tests/test-udp.c`
- Modify: `tests/CMakeLists.txt` (add `netkit_add_test(udp)`)

- [ ] **Step 1: Create `include/netkit/netkit-udp.h`**

```c
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

#include "netkit-types.h"

#include <stdint.h>

/**
 * @brief Create a new UDP handle.
 *
 * @param loop  The event loop.
 *
 * @return New UDP handle, or NULL on failure.
 */
extern netkit_udp_t* netkit_udp_create(netkit_loop_t* loop);

/**
 * @brief Bind the UDP socket to a local address.
 *
 * @param udp   The UDP handle.
 * @param ip    IP address string.
 * @param port  Port number.
 *
 * @return 0 on success, -1 on failure.
 */
extern int netkit_udp_bind(netkit_udp_t* udp, const char* ip, int32_t port);

/**
 * @brief Start receiving datagrams.
 *
 * @param udp   The UDP handle (must be bound).
 * @param cb    Callback invoked for each received datagram.
 * @param data  User data for callback.
 *
 * @return 0 on success, -1 on failure.
 */
extern int netkit_udp_recv(
    netkit_udp_t* udp,
    netkit_udp_recv_fn_t cb,
    void* data);

/**
 * @brief Send a datagram to a remote address.
 *
 * @param udp   The UDP handle.
 * @param buf   Data buffer.
 * @param len   Number of bytes to send.
 * @param ip    Destination IP address.
 * @param port  Destination port.
 * @param cb    Callback invoked when send completes.
 * @param data  User data for callback.
 *
 * @return 0 on success, -1 on failure.
 */
extern int netkit_udp_send(
    netkit_udp_t* udp,
    const void* buf,
    int32_t len,
    const char* ip,
    int32_t port,
    netkit_udp_send_fn_t cb,
    void* data);

/**
 * @brief Close the UDP handle asynchronously.
 *
 * @param udp   The UDP handle.
 * @param cb    Close callback (may be NULL).
 * @param data  User data for close callback.
 */
extern void netkit_udp_close(
    netkit_udp_t* udp,
    netkit_close_fn_t cb,
    void* data);
```

- [ ] **Step 2: Replace `src/netkit-udp.h`**

Replace the entire file:

```c
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
#include "op.h"

#include <winsock2.h>
#include <windows.h>

struct netkit_udp_s {
    _handle_t  base;  /**< Embedded handle base. */
    SOCKET     sock;  /**< Winsock UDP socket. */
};

/**
 * @brief Handle WSARecvFrom completion.
 */
extern void _udp_on_recv(_op_t* op, DWORD bytes, BOOL success);

/**
 * @brief Handle WSASendTo completion.
 */
extern void _udp_on_send(_op_t* op, BOOL success);
```

- [ ] **Step 3: Replace `src/netkit-udp.c`**

Replace the entire stub file:

```c
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

#include "netkit/netkit-udp.h"
#include "netkit/netkit-err.h"

#include "netkit-udp.h"
#include "netkit-loop.h"
#include "netkit-err.h"

#include <ws2tcpip.h>
#include <stdlib.h>
#include <string.h>

/* Extended op for UDP recv that stores the sender address. */
typedef struct _udp_recv_op_s {
    _op_t              base;                    /**< Base op (OVERLAPPED first). */
    struct sockaddr_in from_addr;               /**< Sender address filled by WSARecvFrom. */
    int                from_len;                /**< Size of from_addr. */
} _udp_recv_op_t;

/* Extended op for UDP send that stores the destination address. */
typedef struct _udp_send_op_s {
    _op_t              base;                    /**< Base op (OVERLAPPED first). */
    struct sockaddr_in to_addr;                 /**< Destination address. */
} _udp_send_op_t;

netkit_udp_t* netkit_udp_create(netkit_loop_t* loop) {
    if (!loop) {
        return NULL;
    }

    SOCKET sock = WSASocketW(
        AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (sock == INVALID_SOCKET) {
        return NULL;
    }

    netkit_udp_t* udp = (netkit_udp_t*)calloc(1, sizeof(netkit_udp_t));
    if (!udp) {
        closesocket(sock);
        return NULL;
    }

    udp->base.type = _HANDLE_UDP;
    udp->base.loop = loop;
    udp->sock      = sock;

    if (_loop_associate(loop, sock) != 0) {
        closesocket(sock);
        free(udp);
        return NULL;
    }

    _loop_handle_register(loop);
    return udp;
}

int netkit_udp_bind(netkit_udp_t* udp, const char* ip, int32_t port) {
    if (!udp || !ip || udp->base.closing) {
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons((uint16_t)port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1) {
        return -1;
    }

    if (bind(udp->sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        return -1;
    }
    return 0;
}

int netkit_udp_recv(
    netkit_udp_t* udp,
    netkit_udp_recv_fn_t cb,
    void* data) {

    if (!udp || !cb || udp->base.closing) {
        return -1;
    }

    _udp_recv_op_t* rop = (_udp_recv_op_t*)calloc(1, sizeof(_udp_recv_op_t));
    if (!rop) {
        return -1;
    }

    rop->base.type      = _OP_RECV;
    rop->base.handle    = udp;
    rop->base.cb        = (void*)cb;
    rop->base.user_data = data;
    rop->base.accept_sock = INVALID_SOCKET;
    rop->base.wsa_buf.buf = (char*)rop->base.read_buf;
    rop->base.wsa_buf.len = OP_READ_BUF_SIZE;
    rop->from_len         = (int)sizeof(rop->from_addr);

    DWORD flags = 0;
    int rc = WSARecvFrom(
        udp->sock,
        &rop->base.wsa_buf,
        1,
        NULL,
        &flags,
        (struct sockaddr*)&rop->from_addr,
        &rop->from_len,
        &rop->base.overlapped,
        NULL);

    if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        free(rop);
        return -1;
    }

    udp->base.pending_ops++;
    return 0;
}

int netkit_udp_send(
    netkit_udp_t* udp,
    const void* buf,
    int32_t len,
    const char* ip,
    int32_t port,
    netkit_udp_send_fn_t cb,
    void* data) {

    if (!udp || !buf || len <= 0 || !ip || udp->base.closing) {
        return -1;
    }

    _udp_send_op_t* sop = (_udp_send_op_t*)calloc(1, sizeof(_udp_send_op_t));
    if (!sop) {
        return -1;
    }

    sop->base.type      = _OP_SEND;
    sop->base.handle    = udp;
    sop->base.cb        = (void*)cb;
    sop->base.user_data = data;
    sop->base.accept_sock = INVALID_SOCKET;
    sop->base.wsa_buf.buf = (char*)buf;
    sop->base.wsa_buf.len = (ULONG)len;

    memset(&sop->to_addr, 0, sizeof(sop->to_addr));
    sop->to_addr.sin_family = AF_INET;
    sop->to_addr.sin_port   = htons((uint16_t)port);
    if (inet_pton(AF_INET, ip, &sop->to_addr.sin_addr) != 1) {
        free(sop);
        return -1;
    }

    int rc = WSASendTo(
        udp->sock,
        &sop->base.wsa_buf,
        1,
        NULL,
        0,
        (struct sockaddr*)&sop->to_addr,
        sizeof(sop->to_addr),
        &sop->base.overlapped,
        NULL);

    if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        free(sop);
        return -1;
    }

    udp->base.pending_ops++;
    return 0;
}

void netkit_udp_close(
    netkit_udp_t* udp,
    netkit_close_fn_t cb,
    void* data) {

    if (!udp || udp->base.closing) {
        return;
    }

    udp->base.closing    = true;
    udp->base.close_cb   = cb;
    udp->base.close_data = data;

    CancelIoEx((HANDLE)udp->sock, NULL);
    closesocket(udp->sock);
    udp->sock = INVALID_SOCKET;

    if (udp->base.pending_ops == 0) {
        if (cb) {
            cb(udp, data);
        }
        _loop_handle_unregister(udp->base.loop);
        free(udp);
    }
}

void _udp_on_recv(_op_t* op, DWORD bytes, BOOL success) {
    netkit_udp_t* udp = (netkit_udp_t*)op->handle;
    netkit_udp_recv_fn_t cb = (netkit_udp_recv_fn_t)op->cb;
    _udp_recv_op_t* rop = (_udp_recv_op_t*)op;

    if (!success || bytes == 0) {
        if (cb && !udp->base.closing) {
            cb(udp, NULL, netkit_err_from_wsa(), NULL, 0, op->user_data);
        }
        return;
    }

    /* Convert sender address to string. */
    char addr_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &rop->from_addr.sin_addr, addr_str, sizeof(addr_str));
    int32_t from_port = (int32_t)ntohs(rop->from_addr.sin_port);

    if (cb) {
        cb(udp, op->read_buf, (int32_t)bytes, addr_str, from_port, op->user_data);
    }
}

void _udp_on_send(_op_t* op, BOOL success) {
    netkit_udp_t* udp = (netkit_udp_t*)op->handle;
    netkit_udp_send_fn_t cb = (netkit_udp_send_fn_t)op->cb;

    if (cb) {
        cb(udp, success ? 0 : -1, op->user_data);
    }
}
```

- [ ] **Step 4: Write test `tests/test-udp.c`**

```c
#include "netkit.h"
#include "assert.h"

#include <string.h>

#define TEST_PORT 19877
#define TEST_MSG  "hello udp"

static netkit_loop_t* _loop = NULL;

static void _on_safety_timeout(netkit_timer_t* timer, void* data) {
    (void)data;
    netkit_timer_close(timer, NULL, NULL);
    netkit_loop_stop(_loop);
    ASSERT(0);
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
    netkit_timer_t* safety = netkit_timer_create(_loop);
    ASSERT(safety != NULL);
    netkit_timer_start(safety, 5000, 0, _on_safety_timeout, NULL);

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
```

- [ ] **Step 5: Register test in `tests/CMakeLists.txt`**

Add after `netkit_add_test(tcp)`:

```cmake
netkit_add_test(udp)
```

- [ ] **Step 6: Build and run all tests**

Run:
```bash
cmake -B out -DNETKIT_ENABLE_TESTING=ON
cmake --build out --config Debug
ctest --test-dir out -C Debug --output-on-failure
```

Expected: all tests PASS (err, heap, loop, timer, tcp, udp)

- [ ] **Step 7: Commit**

```bash
git add -A
git commit -m "feat: add UDP module with IOCP-based send and recv"
```

---

### Task 9: Final Integration and Cleanup

**Files:**
- Verify: `include/netkit.h` (should already include all module headers from Task 1)
- Verify: `CMakeLists.txt` (all source files in SRCS)
- Verify: `tests/CMakeLists.txt` (all test modules registered)

- [ ] **Step 1: Verify umbrella header includes all modules**

`include/netkit.h` should contain (from Task 1):

```c
#include "netkit/netkit-types.h"
#include "netkit/netkit-err.h"
#include "netkit/netkit-loop.h"
#include "netkit/netkit-tcp.h"
#include "netkit/netkit-udp.h"
#include "netkit/netkit-timer.h"
```

If any are missing, add them.

- [ ] **Step 2: Verify CMakeLists.txt SRCS**

The root `CMakeLists.txt` should have:

```cmake
set(SRCS
    src/netkit-err.c
    src/heap.c
    src/op.c
    src/netkit-loop.c
    src/netkit-tcp.c
    src/netkit-udp.c
    src/netkit-timer.c
)
```

- [ ] **Step 3: Verify tests/CMakeLists.txt**

Should contain:

```cmake
netkit_add_test(err)
netkit_add_test(heap)
netkit_add_test(loop)
netkit_add_test(timer)
netkit_add_test(tcp)
netkit_add_test(udp)
```

- [ ] **Step 4: Full build and test run**

Run:
```bash
cmake -B out -DNETKIT_ENABLE_TESTING=ON
cmake --build out --config Debug
ctest --test-dir out -C Debug --output-on-failure
```

Expected: 6 tests, all PASS.

- [ ] **Step 5: Commit**

```bash
git add -A
git commit -m "feat: complete netkit async I/O library integration"
```
