# Build Instructions

This guide covers building, testing, and generating coverage reports on Windows and Unix using CMake.

## Prerequisites

- CMake >= 3.16
- A C11-compatible compiler:
  - Windows: MSVC (Visual Studio 2022+) or Clang-cl
  - Linux/macOS: GCC >= 7 or Clang >= 6

## Configure and Build

```bash
cmake -B out
cmake --build out
```

## Run Tests

```bash
ctest --test-dir out --output-on-failure
```

## Sanitizers

```bash
cmake -B out -DMY_UTILS_ENABLE_ASAN=ON
cmake --build out
```
