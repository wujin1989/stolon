# Build Instructions

This guide covers building, testing, and generating coverage reports on Windows and Unix using CMake.

## Prerequisites

- CMake >= 3.25
- A C11-compatible compiler:
  - Windows: MSVC (Visual Studio 2022+) or Clang-cl
  - Linux/macOS: GCC >= 7 or Clang >= 6
- (Optional) For code coverage:
  - Linux: `lcov` and `genhtml` (`sudo apt install lcov`)
  - Windows: OpenCppCoverage (`winget install OpenCppCoverage.OpenCppCoverage`)

## Configure

### Multi-Config Generators (Visual Studio, Ninja Multi-Config)

```bash
cmake -B out
```

### Single-Config Generators (Unix Makefiles, Ninja)

```bash
cmake -B out -DCMAKE_BUILD_TYPE=Debug
```

## Build

### Multi-Config

```bash
cmake --build out --config Debug -j 8
```

### Single-Config

```bash
cmake --build out -j 8
```

## Run Tests

### Multi-Config

```bash
ctest --test-dir out -C Debug --output-on-failure
```

### Single-Config

```bash
ctest --test-dir out --output-on-failure
```

### Running a Single Test

```bash
ctest --test-dir out -R <module> --output-on-failure
```

## Sanitizers

```bash
cmake -B out -DSVCMON_ENABLE_ASAN=ON
cmake --build out
```

| Sanitizer | What it catches | Option |
|-----------|----------------|--------|
| ASAN | Buffer overflow, use-after-free, memory leaks | `-DSVCMON_ENABLE_ASAN=ON` |
| TSAN | Data races, deadlocks | `-DSVCMON_ENABLE_TSAN=ON` |
| UBSAN | Undefined behavior | `-DSVCMON_ENABLE_UBSAN=ON` |

ASAN and TSAN cannot be enabled simultaneously.

## Code Coverage

### Linux/macOS

```bash
cmake -B out -DSVCMON_ENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build out -j 8
cmake --build out --target coverage
```

### Windows

```bash
cmake -B out -DSVCMON_ENABLE_COVERAGE=ON
cmake --build out --config Debug
cmake --build out --target coverage
```

HTML report at `out/coverage/html/index.html`.

## Quick Reference

| Step | Multi-Config | Single-Config |
|------|-------------|---------------|
| Configure | No `-DCMAKE_BUILD_TYPE` | Must set `-DCMAKE_BUILD_TYPE=Debug` |
| Build | `--build ... --config Debug` | `--build ...` |
| Test | `ctest ... -C Debug` | `ctest ...` |
