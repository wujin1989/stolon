# Build Instructions

## Prerequisites

- CMake >= 3.16
- C11 compiler (MSVC, GCC, or Clang)

### Platform-Specific

- **Windows:** Visual Studio 2022 with C++ workload
- **Linux:** `sudo apt install build-essential cmake`
- **macOS:** `brew install cmake` + Xcode Command Line Tools

Or run the install script:
- Linux/macOS: `./scripts/install-deps.sh`
- Windows: `scripts/install-deps.ps1` (requires admin)

## Configure & Build

```bash
cmake -B out
cmake --build out
```

## Run Tests

```bash
# Windows (multi-config)
ctest --test-dir out -C Debug --output-on-failure

# Linux/macOS (single-config)
ctest --test-dir out --output-on-failure
```

## Sanitizers

```bash
cmake -B out -D{PROJECT}_ENABLE_ASAN=ON
cmake --build out
```

## Coverage

```bash
# Linux/macOS
cmake -B out -D{PROJECT}_ENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build out
cmake --build out --target coverage

# Windows (requires OpenCppCoverage)
cmake -B out -D{PROJECT}_ENABLE_COVERAGE=ON
cmake --build out --config Debug
cmake --build out --target coverage
```
