# Code Style

> Recommended trigger: auto-include when editing `*.c` / `*.h` files.

## Naming Convention

**Prefix Rule:** Use your project name in lowercase as the namespace prefix.

| Category | Pattern | Example (project: `mylib`) |
|----------|---------|---------------------------|
| Public functions | `{project}_<module>_<action>` | `mylib_list_insert` |
| Types | `{project}_<module>_t` | `mylib_list_t` |
| Internal/static helpers | `_<name>` prefix | `_helper_function` |
| Source files | `{project}-<module>.c` | `mylib-list.c` |

> **For this project:** Replace `{project}` with your actual project name.
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

## Project Structure

```
include/{project}/{project}-<module>.h  # Public API
src/{project}-<module>.c                # Implementation
src/platform/win/                       # Windows platform code
src/platform/unix/                      # Linux/macOS platform code
tests/test-<module>.c                   # Unit tests
```

### Adding a Module
1. Create `include/{project}/{project}-<module>.h` with public API
2. Create `src/{project}-<module>.c` with implementation
3. Add to `SRCS` in root `CMakeLists.txt`
4. Include in `include/{project}.h`
5. Create `tests/test-<module>.c`
6. Add `{project}_add_test(<module>)` to `tests/CMakeLists.txt`

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
