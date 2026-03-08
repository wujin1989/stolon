# Dependencies

> This file defines dependency diagnosis and resolution procedures for AI agents when builds fail due to missing software, libraries, or tools.

## Diagnosis Order

1. Read the error message — identify the missing tool, library, or header
2. Check the table below for known dependencies and install commands
3. Verify the installed version meets the minimum requirement
4. Re-run the failed command to confirm the fix

## Required Tools

| Tool | Min Version | Purpose | Check Command |
|------|-------------|---------|---------------|
| CMake | 3.16 | Build system | `cmake --version` |
| C compiler | C11 support | Compilation | `gcc --version` / `cl` / `clang --version` |
| clang-format | — | Code formatting | `clang-format --version` |

## Install Commands

### CMake

| Platform | Command |
|----------|---------|
| Ubuntu/Debian | `sudo apt install cmake` |
| macOS | `brew install cmake` |
| Windows | `winget install Kitware.CMake` or [cmake.org](https://cmake.org/download/) |

### C Compiler

| Platform | Compiler | Command |
|----------|----------|---------|
| Ubuntu/Debian | GCC | `sudo apt install build-essential` |
| Ubuntu/Debian | Clang | `sudo apt install clang` |
| macOS | Apple Clang | `xcode-select --install` |
| Windows | MSVC | Install Visual Studio 2022 with "Desktop development with C++" workload |

### clang-format

| Platform | Command |
|----------|---------|
| Ubuntu/Debian | `sudo apt install clang-format` |
| macOS | `brew install clang-format` |
| Windows | Included with LLVM: [llvm.org](https://releases.llvm.org/) |

## Optional Tools

### Coverage

| Platform | Tool | Command |
|----------|------|---------|
| Linux | lcov + genhtml | `sudo apt install lcov` |
| macOS | lcov + genhtml | `brew install lcov` |
| Windows | OpenCppCoverage | [github.com/OpenCppCoverage](https://github.com/OpenCppCoverage/OpenCppCoverage) |

### Debuggers

| Platform | Tool | Command |
|----------|------|---------|
| Linux | GDB | `sudo apt install gdb` |
| macOS | LLDB | Included with Xcode CLT (`xcode-select --install`) |
| Windows | WinDbg | `winget install Microsoft.WinDbg` or Microsoft Store |

## Common Errors

| Error | Cause | Fix |
|-------|-------|-----|
| `cmake: command not found` | CMake not installed | Install CMake (see above) |
| `No CMAKE_C_COMPILER could be found` | No C compiler in PATH | Install a C compiler (see above) |
| `fatal error: xxx.h: No such file or directory` | Missing system header or library dev package | `sudo apt install lib<name>-dev` (Linux) / `brew install <name>` (macOS) |
| `cannot find -l<name>` | Missing library at link time | Install the library dev package or set `CMAKE_PREFIX_PATH` |
| `clang-format: command not found` | clang-format not installed | Install clang-format (see above) |
| `lcov: command not found` | lcov not installed (coverage build) | Install lcov (see above) |

## Resolution Rules

| Rule | Detail |
|------|--------|
| Check version after install | Run the check command from the Required Tools table to confirm the version meets the minimum |
| Prefer system package manager | Use `apt`, `brew`, or `winget` before manual downloads |
| Do not modify CMakeLists.txt to work around missing tools | Install the tool instead |
| Document non-standard install paths | If a tool is installed outside PATH, set `CMAKE_PREFIX_PATH` or the tool-specific environment variable |
