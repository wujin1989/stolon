# Debugging Guide

> Recommended trigger: manual include.

## Platform Debuggers

| Platform | Debugger | Command |
|----------|----------|---------|
| Linux | GDB | `gdb ./program` |
| macOS | LLDB | `lldb ./program` |
| Windows | MSVC | Use Visual Studio or `devenv /debug program.exe` |

## Common Commands

### GDB / LLDB

| Action | GDB | LLDB |
|--------|-----|------|
| Set breakpoint | `b main` | `b main` |
| Run | `r` | `r` |
| Step over | `n` | `n` |
| Step into | `s` | `s` |
| Print variable | `p var` | `p var` |
| Backtrace | `bt` | `bt` |
| Continue | `c` | `c` |
| Watch variable | `watch var` | `w set var var` |

## Cross-Platform Pitfalls

- `long` is 4 bytes on Windows x64, 8 bytes on Linux/macOS x64 — watch for truncation
- Stack size defaults differ: Windows 1MB, Linux 8MB
- Uninitialized memory may be zeroed in debug builds but not release — use ASAN
- Signal handling differs: Windows uses SEH, Unix uses POSIX signals
- File path separators: `\` on Windows, `/` on Unix

## Debug Build Configuration

```bash
# Debug build with symbols
cmake -B out -DCMAKE_BUILD_TYPE=Debug

# With ASAN for memory error detection
cmake -B out -D{PROJECT}_ENABLE_ASAN=ON
```

## Tips

- Always reproduce with a minimal test case first
- Use sanitizers (ASAN/TSAN/UBSAN) before reaching for a debugger
- Check compiler warnings — many bugs are caught at compile time
- On Windows, use Debug configuration (`-C Debug`) for full symbol info
- Core dumps: `ulimit -c unlimited` on Linux, then `gdb ./program core`
