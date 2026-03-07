# Build System

> Recommended trigger: auto-include when editing `CMakeLists.txt` / `*.cmake` files.

## Stack
- C11, CMake >= 3.16
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
cmake -B out && cmake --build out
ctest --test-dir out -C Debug --output-on-failure  # Windows
ctest --test-dir out --output-on-failure           # Linux/macOS
```

## Troubleshooting
- Missing coverage tool: run `./scripts/install-deps.sh` (Linux/macOS) or `scripts/install-deps.ps1` (Windows)
- Linux/macOS needs: lcov, genhtml
- Windows needs: OpenCppCoverage

## Test Framework

Custom `ASSERT(expr)` macro in `tests/assert.h` — prints `file:line` and aborts on failure.

Tests are plain C executables: `main()` calls `static void test_*()` functions. Registered via `{project}_add_test(<name>)` in CMake, which creates `test-<name>` from `test-<name>.c` linked against `{project}`.
