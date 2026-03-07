# Code Style

> Recommended trigger: auto-include when editing `*.c` / `*.h` files.

## License Header

Every `.c` and `.h` file must start with the project license block:

```c
/** Copyright (c) {YEAR}, {AUTHOR} <{EMAIL}>
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

## Naming Convention

**Prefix Rule:** Use your project name in lowercase as the namespace prefix.

| Category | Pattern | Example (project: `mylib`) |
|----------|---------|---------------------------|
| Public functions | `{project}_<module>_<action>` | `mylib_list_insert` |
| Types | `{project}_<module>_t` | `mylib_list_t` |
| Internal/static helpers | `_<name>` prefix | `_helper_function` |
| Source files | `{project}-<module>.c` | `mylib-list.c` |

Action (verb) goes last: `{project}_list_insert`, `{project}_heap_remove`.
Compound actions stay together: `{project}_timer_set_time` (not `{project}_timer_time_set`).

> The `_` prefix for static functions is technically reserved by C11 but commonly used (symbols never exported).

## Types

- Use fixed-width types (`int32_t`, `uint8_t`) over plain `int`/`unsigned`
- Exception: `int` for return codes, `size_t` for sizes, `bool` for flags
- For printf: use `<inttypes.h>` macros (`PRIu64`, `PRId32`, `PRIx32`) instead of `%lu`, `%llu`
  - Example: `printf("value: %" PRIu64 "\n", my_uint64);`
  - Reason: `long` size varies across platforms (Windows 64-bit: 4 bytes, Linux 64-bit: 8 bytes)

## File Organization

Order: License → includes → macros → structs → static functions → public functions

Static functions ordered by dependency (no forward declarations).

## Project Structure (Library)

```
include/{project}/{project}-<module>.h  # Public API
src/{project}-<module>.c                # Implementation
src/platform/win/                       # Windows platform code
src/platform/unix/                      # Linux/macOS platform code
tests/test-<module>.c                   # Unit tests
examples/                               # Example programs
```

## Project Structure (Executable)

```
src/{project}-<module>.c                # Implementation
src/{project}-<module>.h                # Internal headers (alongside .c)
src/main.c                              # Entry point
src/platform/win/                       # Windows platform code
src/platform/unix/                      # Linux/macOS platform code
tests/test-<module>.c                   # Unit tests
```

### Adding a Module (Library)
1. Create `include/{project}/{project}-<module>.h` with public API
2. Create `src/{project}-<module>.c` with implementation
3. Add to `SRCS` in root `CMakeLists.txt`
4. Include in `include/{project}.h`
5. Create `tests/test-<module>.c`
6. Add `{project}_add_test(<module>)` to `tests/CMakeLists.txt`
7. When adding files to `src/platform/win/` or `src/platform/unix/`, delete `.gitkeep` in that directory if it exists

### Adding a Module (Executable)
1. Create `src/{project}-<module>.c` and `src/{project}-<module>.h`
2. Add to `SRCS` in root `CMakeLists.txt`
3. Create `tests/test-<module>.c`
4. Add `{project}_add_test(<module>)` to `tests/CMakeLists.txt`
5. When adding files to `src/platform/win/` or `src/platform/unix/`, delete `.gitkeep` in that directory if it exists

## Headers

- Use `_Pragma("once")` for header guards

## Formatting

- 4-space indent, pointer left (`int* p`)
- Always use braces for control statements (exception: `if (!ptr) return -1;`)

## Comments

- Public API: Doxygen `/** @brief ... @param ... @return ... */`
- Static functions: `/* one-liner */` if needed
- Multi-line: `/** \n * line1 \n * line2 \n */`
- No decorative dividers
