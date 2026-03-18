
# Code Style

## License Header

Every `.c` and `.h` file must start with the project license text as a `/** ... */` comment. Read the content from the project root `LICENSE` file.

## Naming Convention

### Prefix Rule

`<project>` is the project name from `CMakeLists.txt`.

| Category | Pattern |
|----------|---------|
| Public functions | `<project>_<module>_<action>` |
| Static functions | `_<module>_<action>` |
| Static callbacks | `_<module>_<subject>_<event>_cb` |
| Types | `<project>_<module>_t` |
| Node types | `<project>_<module>_node_t` |
| Function pointer typedefs | `<project>_<module>_<purpose>_fn_t` |
| Internal types (file-scope) | `_<name>_t` |
| Static variables (file-scope) | `_<name>` |
| Global variables (non-static, in `.c` files) | `snake_case`, no prefix |
| Source files | `<project>-<module>.c` |
| Test files | `test-<module>.c` |
| Include directory | `include/<project>/` |
| Umbrella header | `include/<project>.h` |

> The `_` prefix for file-scope static functions and internal types is technically reserved by C11 (§7.1.3), but is used intentionally here. These symbols are never exported and do not enter the linker symbol table, so conflicts with the implementation are not a practical concern.

### Public Functions

Three segments: `<project>`, `<module>`, `<action>`. Module is a single identifier without underscores. Action may be compound (verb + object with `_`), verb first:
- `<project>_list_insert` — module=`list`, action=`insert`
- `<project>_loop_init_timer` — module=`loop`, action=`init_timer`
- `<project>_timer_set_time` — module=`timer`, action=`set_time`

### Static Functions

Two segments: `<module>`, `<action>`, prefixed with `_`. Same rules as public:
- `_heap_swap_node` — module=`heap`, action=`swap_node`
- `_tcp_flush_writes` — module=`tcp`, action=`flush_writes`

### Static Callbacks

Three segments: `<module>`, `<subject>`, `<event>`, with `_cb` suffix. Subject names the resource being monitored, not the mechanism. These describe events, not actions, so the verb-first rule does not apply:
- `_tcp_conn_io_cb` — module=`tcp`, subject=`conn`, event=`io`
- `_tcp_write_timeout_cb` — module=`tcp`, subject=`write`, event=`timeout`
- `_tcp_reconnect_timeout_cb` — module=`tcp`, subject=`reconnect`, event=`timeout`

## Types

- Use fixed-width integer types (`int8_t`, `uint8_t`, etc.) instead of plain `int`/`unsigned`
- Exception: function return values, loop counters, and parameters may use `int` for error codes, comparator results, and boolean-like flags
- Use `size_t` for sizes and counts, `bool` for flags
- For printf: use `<inttypes.h>` macros (`PRIu64`, `PRId32`, `PRIx32`) instead of `%lu`, `%llu`
  - Example: `printf("value: %" PRIu64 "\n", my_uint64);`
  - Reason: `long` size varies across platforms (Windows 64-bit: 4 bytes, Linux 64-bit: 8 bytes)

## Lifecycle Naming

Memory ownership determines the naming pattern:

| Owner | Create | Destroy | Usage |
|-------|--------|---------|-------|
| Caller (stack/embedded) | `init` | `deinit` | Intrusive types, short-lived objects |
| Module (malloc inside) | `create` | `destroy` | Opaque types, dynamic lifetime |

- `init`/`deinit` — caller provides memory, module only initializes/cleans up fields
- `create`/`destroy` — module allocates and frees the struct internally

Rule: opaque types must use `create`/`destroy` (caller can't `sizeof`). Intrusive types must use `init`/`deinit` (memory belongs to caller).

```c
/* init/deinit — caller owns the memory */
<project>_list_t list;
<project>_list_init(&list);
/* ... use list ... */
<project>_list_deinit(&list);

/* create/destroy — module owns the memory */
<project>_logger_t* logger = <project>_logger_create(cfg);
/* ... use logger ... */
<project>_logger_destroy(logger);
```

### Related Verb Pairs

| Pair | Semantics | When to use |
|------|-----------|-------------|
| `open` / `close` | Access an external resource (file, connection, device) | The resource already exists; you're obtaining a handle to it |
| `alloc` / `free` | Allocate raw memory, minimal or no initialization | Low-level allocators, memory pools |
| `start` / `stop` | Control running state (repeatable) | Timers, workers, polling — can start/stop multiple times within one init/deinit lifecycle |
| `acquire` / `release` | Obtain/return ownership or reference count | Reference-counted resources, shared handles |
| `register` / `unregister` | Add/remove a callback or handler to a system | Event handlers, hooks, observers |
| `attach` / `detach` | Associate/dissociate with a parent object | Mounting a child handle onto a loop or manager |
| `bind` / `unbind` | Associate with an address, port, or resource | Network sockets, device endpoints |
| `flush` | Push buffered data to its destination | Write buffers, log buffers — data is sent out, buffer may refill |
| `drain` | Process/consume all pending items until empty | Work queues, event queues — items are consumed, queue ends up empty |

## Opaque Structs

Non-intrusive types that users interact with only through pointers (handles) must use the opaque pattern:
- Header: forward declaration + typedef only (`typedef struct <project>_foo_s <project>_foo_t;`)
- Implementation (.c): full struct definition with fields
- Users allocate via `create()` / module-specific constructors, never `sizeof()`

Intrusive data structures (list, queue, stack, heap, rbtree, etc.) where users embed nodes into their own structs are exempt — their struct bodies must remain in headers.

## File Organization

Order: License -> includes -> macros -> structs -> variables -> static functions -> public functions

Static functions ordered by dependency (no forward declarations).

## Platform Layer

All platform-specific code lives exclusively under `src/platform/`. Source files outside `platform/` must not contain platform conditionals (`#ifdef _WIN32`, `#ifdef __linux__`, etc.).

Prefer cross-platform APIs wherever possible. Use the platform layer only when no cross-platform equivalent exists.

| Rule | Detail |
|------|--------|
| Umbrella header | `src/platform/platform.h` only includes per-module headers — no declarations directly in it |
| Per-module header | `src/platform/platform-<module>.h` declares that module's `platform_*` functions |
| Prefix | `platform_<module>_<action>` (e.g. `platform_time_monotonic_nsec`) |
| Implementation | One `.c` per module per platform (`src/platform/win/platform-<module>.c`, `src/platform/unix/platform-<module>.c`) |
| Scope | Only the minimal OS-specific logic; everything else stays in `src/<project>-<module>.c` |

Public API functions belong in `src/<project>-<module>.c` and call `platform_*` helpers for OS-dependent parts. Never put public API implementations directly in platform files.

## Adding a Module

### Library
1. If `include/<project>/` does not exist, create it
2. Create `include/<project>/<project>-<module>.h` with public API
3. Create `src/<project>-<module>.c` with implementation
4. Add to `SRCS` in root `CMakeLists.txt`
5. Include in `include/<project>.h`
6. Create `tests/test-<module>.c`
7. Add `<project>_add_test(<module>)` to `tests/CMakeLists.txt`
8. When adding files to `src/platform/win/` or `src/platform/unix/`, delete `.gitkeep` in that directory if it exists

### Executable
1. Create `src/<project>-<module>.c` and `src/<project>-<module>.h`
2. Add to `SRCS` in root `CMakeLists.txt`
3. Create `tests/test-<module>.c`
4. Add `<project>_add_test(<module>)` to `tests/CMakeLists.txt`
5. When adding files to `src/platform/win/` or `src/platform/unix/`, delete `.gitkeep` in that directory if it exists

## Restricted Functions

Applies to all `.c` and `.h` files in the project (including tests).

| Banned | Replacement | Reason |
|--------|-------------|--------|
| `localtime`, `gmtime`, `ctime`, `asctime`, `strtok`, `strerror` | `_r` (POSIX) / `_s` (MSVC) variants | Return static buffer — not thread-safe, successive calls overwrite results |
| `sprintf`, `strcpy`, `strcat` | `snprintf` | No bounds checking — buffer overflow risk |
| `gets` | `fgets` | Removed in C11 — unconditional buffer overflow |
| `atoi`, `atof`, `atol` | `strtol`, `strtod` | No error detection — overflow is undefined behavior |
| `memcpy` with overlapping regions | `memmove` | Overlapping src/dst is undefined behavior |

## Headers

- Use `_Pragma("once")` for header guards

## Formatting

clang-format with LLVM base style (see `.clang-format` for full config). Additional rule beyond the formatter:
- All `if`/`else`/`for`/`while` bodies must use braces, even single statements

## Comments

### Public API (Header Files)

All `extern` function declarations must have a Doxygen `/** ... */` block:

```c
/**
 * @brief One-line summary.
 *
 * Optional details about behavior or algorithm.
 *
 * @param name   Description of the parameter.
 * @param name2  Aligned with other @param entries.
 *
 * @return Return value and error conditions.
 *
 * @note Caller responsibilities, buffer sizing, etc.
 */
```

Rules:
- Use `@brief`, `@param`, `@return`, `@note` tags
- Align `@param` descriptions
- Pure ASCII only — use `->` not `→`, `>=` not `≥`
- Blank comment lines between sections

### Internal / Static Functions

No Doxygen tags required. Use `/** ... */` for multi-line comments, `/* ... */` for single-line:

```c
/**
 * Common setup for a newly connected socket: init ringbuf, start IO,
 * start heartbeat/read timers. Does NOT call any handler callback.
 */
static void _tcp_setup_conn(...) { ... }

/* Swap two heap nodes and update their positions. */
static inline void _heap_swap_node(...) { ... }
```

### Inline Comments

Applies to comments inside function bodies in `.c` files.

- `/* ... */` style (C11 compatible)
- `/** ... */` when spanning multiple lines (block format: `/**` and `*/` on own lines)
- A comment must explain *why* the code does something, not *what* it does. If the comment restates the code, remove it.
- No decorative dividers (lines of `===`, `---`, `***`, etc.)

```c
/* Correct — explains why */
/* Reserve extra byte for null terminator required by snprintf. */
uint8_t buf[len + 1];

/* Incorrect — restates the code */
/* Add 1 to len. */
uint8_t buf[len + 1];
```
