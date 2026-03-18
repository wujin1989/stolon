
# Testing

## File Scope

Applies to `tests/test-<module>.c` files and `tests/CMakeLists.txt`.

## Framework

No external test framework. Tests use the custom `ASSERT(expr)` macro from `tests/assert.h`. Do not use standard `<assert.h>` or any third-party test library.

## Test File Structure

One test file per module: `tests/test-<module>.c`. Each file follows this structure:

```c
#include "assert.h"
#include "<project>-<module>.h"  /* or the relevant header */

static void test_<case>(void) {
    /* setup */
    /* action */
    /* assert */
    ASSERT(result == expected);
}

int main(void) {
    test_<case>();
    return 0;
}
```

| Rule | Detail |
|------|--------|
| Entry point | `main()` calls `static void test_*()` functions sequentially |
| Return value | `main()` returns `0` on success; `ASSERT` calls `abort()` on failure |
| No test runner | Each test executable is self-contained — no shared `main()` across modules |

## Naming

| Category | Pattern | Example |
|----------|---------|---------|
| Test file | `test-<module>.c` | `test-list.c` |
| Test function | `static void test_<case>(void)` | `test_insert`, `test_remove_empty` |
| Test executable | `test-<module>` (created by CMake) | `test-list` |

Test function names use `snake_case`. The `<case>` part describes the scenario, not the function under test:
- `test_insert` ✓ — tests the insert operation
- `test_insert_duplicate` ✓ — tests inserting a duplicate
- `test_mylib_list_insert` ✗ — do not repeat the module/project prefix

## Registration

Add each test module in `tests/CMakeLists.txt` using the helper function:

```cmake
<project>_add_test(<module>)
```

This creates executable `test-<module>` from `tests/test-<module>.c`, linked against `<project>`, with sanitizers and coverage applied automatically.

Do not create test executables manually with `add_executable` in `tests/CMakeLists.txt`.

## Test Organization

### One Concern Per Function

Each `test_*` function tests one behavior. Do not combine multiple unrelated assertions in a single function.

```c
/* Correct — one concern */
static void test_insert(void) {
    <project>_list_t list;
    <project>_list_init(&list);
    int32_t rc = <project>_list_insert(&list, 42);
    ASSERT(rc == 0);
    ASSERT(<project>_list_size(&list) == 1);
    <project>_list_deinit(&list);
}

/* Correct — separate concern */
static void test_insert_duplicate(void) {
    <project>_list_t list;
    <project>_list_init(&list);
    <project>_list_insert(&list, 42);
    int32_t rc = <project>_list_insert(&list, 42);
    ASSERT(rc == -1);
    <project>_list_deinit(&list);
}
```

### Cleanup on Every Path

Every `test_*` function must call `deinit`/`destroy`/`close` for all resources it creates, even if the test is expected to pass. This ensures sanitizers (ASAN) report accurate leak information.

### No Global State

Test functions must not depend on execution order. Each function sets up its own state from scratch. Do not use file-scope variables to share state between test functions.

## Running Tests

Build commands are in `build.md`. Quick reference:

```bash
# Windows (multi-config)
ctest --test-dir out -C Debug --output-on-failure

# Linux/macOS (single-config)
ctest --test-dir out --output-on-failure
```

### Running a Single Test

```bash
ctest --test-dir out -R <module> --output-on-failure
```

Example: `ctest --test-dir out -R list --output-on-failure` runs only `test-list`.

## Sanitizer Testing

Run the full test suite with each sanitizer to catch different classes of bugs. Build commands are in `build.md`.

| Sanitizer | What it catches | Command |
|-----------|----------------|---------|
| ASAN | Buffer overflow, use-after-free, memory leaks | `cmake -B out -D<PROJECT>_ENABLE_ASAN=ON` |
| TSAN | Data races, deadlocks | `cmake -B out -D<PROJECT>_ENABLE_TSAN=ON` |
| UBSAN | Undefined behavior (signed overflow, null deref, etc.) | `cmake -B out -D<PROJECT>_ENABLE_UBSAN=ON` |

ASAN and TSAN cannot be enabled simultaneously. Run them in separate builds.

## Coverage

Build commands are in `build.md`. Coverage requires `<PROJECT>_ENABLE_TESTING=ON` and `<PROJECT>_ENABLE_COVERAGE=ON`.

| Rule | Detail |
|------|--------|
| Target | Every public function must have at least one test that exercises its success path |
| Error paths | Functions with error returns must have tests that trigger each distinct error condition |
| Platform code | Platform-specific code is tested on its native platform only — do not mock platform functions |

## Prohibited Patterns

| Pattern | Why | Fix |
|---------|-----|-----|
| `#include <assert.h>` | Standard assert has no file:line output and may be disabled by `NDEBUG` | Use `#include "assert.h"` (project macro) |
| `sleep()` / `thrd_sleep()` in tests | Flaky timing-dependent tests | Use deterministic synchronization or callbacks |
| `printf` as the only validation | Output is not checked automatically | Use `ASSERT()` to validate results |
| Shared mutable file-scope variables | Creates order-dependent tests | Initialize all state inside each `test_*` function |
