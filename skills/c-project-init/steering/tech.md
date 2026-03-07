# Build System

> Recommended trigger: auto-include when editing `CMakeLists.txt` / `*.cmake` files.

## Stack
- C11, CMake >= 3.16
- `_Pragma("once")` instead of traditional include guards
- C11 atomics (Windows requires `/experimental:c11atomics`)
- Output: `out/`

## Options (Library)

| Option | Default | Purpose |
|--------|---------|---------|
| `{PROJECT}_ENABLE_TESTING` | ON | Unit tests |
| `{PROJECT}_ENABLE_ASAN` | OFF | AddressSanitizer |
| `{PROJECT}_ENABLE_TSAN` | OFF | ThreadSanitizer |
| `{PROJECT}_ENABLE_UBSAN` | OFF | UndefinedBehaviorSanitizer |
| `{PROJECT}_ENABLE_DYNAMIC_LIBRARY` | OFF | Shared lib instead of static |
| `{PROJECT}_ENABLE_COVERAGE` | OFF | Code coverage |

## Options (Executable)

| Option | Default | Purpose |
|--------|---------|---------|
| `{PROJECT}_ENABLE_TESTING` | ON | Unit tests |
| `{PROJECT}_ENABLE_ASAN` | OFF | AddressSanitizer |
| `{PROJECT}_ENABLE_TSAN` | OFF | ThreadSanitizer |
| `{PROJECT}_ENABLE_UBSAN` | OFF | UndefinedBehaviorSanitizer |
| `{PROJECT}_ENABLE_COVERAGE` | OFF | Code coverage |

## Commands
```bash
# Configure & build (tests enabled by default)
cmake -B out
cmake --build out

# Disable tests
cmake -B out -D{PROJECT}_ENABLE_TESTING=OFF

# Run tests — multi-config (Windows/MSVC)
ctest --test-dir out -C Debug --output-on-failure

# Run tests — single-config (Linux/macOS)
ctest --test-dir out --output-on-failure

# Sanitizer build
cmake -B out -D{PROJECT}_ENABLE_ASAN=ON
cmake --build out

# Coverage (Linux)
cmake -B out -D{PROJECT}_ENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build out
cmake --build out --target coverage
# Report: out/coverage/
```

## Troubleshooting
- Missing coverage tool: run `./scripts/install-deps.sh` (Linux/macOS) or `scripts/install-deps.ps1` (Windows)
- Linux/macOS needs: lcov, genhtml
- Windows needs: OpenCppCoverage

## Test Framework

Custom `ASSERT(expr)` macro in `tests/assert.h` — prints `file:line` and aborts on failure.

Tests are plain C executables: `main()` calls `static void test_*()` functions. Registered via `{project}_add_test(<name>)` in CMake, which creates `test-<name>` from `test-<name>.c` linked against `{project}`.
