# Build System

> Recommended trigger: auto-include when editing `CMakeLists.txt` / `*.cmake` files.

## Stack
- C11, CMake >= 3.16
- Output: `out/`

## Options (Library)
- `{PROJECT}_ENABLE_TESTING` (ON) - Unit tests
- `{PROJECT}_ENABLE_ASAN/TSAN/UBSAN` (OFF) - Sanitizers
- `{PROJECT}_ENABLE_DYNAMIC_LIBRARY` (OFF) - Shared lib
- `{PROJECT}_ENABLE_COVERAGE` (OFF) - Coverage

## Options (Executable)
- `{PROJECT}_ENABLE_TESTING` (ON) - Unit tests
- `{PROJECT}_ENABLE_ASAN/TSAN/UBSAN` (OFF) - Sanitizers
- `{PROJECT}_ENABLE_COVERAGE` (OFF) - Coverage

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
