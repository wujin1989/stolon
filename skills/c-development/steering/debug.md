# Debugging

> Recommended trigger: manual include. Use when debugging issues.

## Diagnosis Order

Follow this order before reaching for a debugger:

1. Read the error message — compiler warnings, sanitizer output, assertion failures
2. Reproduce with a minimal test case (`tests/test-<module>.c`)
3. Run with sanitizers (ASAN → TSAN → UBSAN) to catch memory/thread/UB issues
4. Use a debugger only if sanitizers don't reveal the cause

## Sanitizer Usage

```bash
# AddressSanitizer — memory errors (use-after-free, buffer overflow, leaks)
cmake -B out -D{PROJECT}_ENABLE_ASAN=ON
cmake --build out && ctest --test-dir out --output-on-failure

# ThreadSanitizer — data races
cmake -B out -D{PROJECT}_ENABLE_TSAN=ON
cmake --build out && ctest --test-dir out --output-on-failure

# UndefinedBehaviorSanitizer — signed overflow, null deref, alignment
cmake -B out -D{PROJECT}_ENABLE_UBSAN=ON
cmake --build out && ctest --test-dir out --output-on-failure
```

Constraints:
- ASAN and TSAN cannot be enabled simultaneously
- Always build in Debug mode for full symbol info (`-C Debug` on Windows)
- ASAN is available on all platforms (MSVC, GCC, Clang). TSAN and UBSAN are GCC/Clang only.

## Debugger Commands

| Platform | Debugger | Launch |
|----------|----------|--------|
| Windows | CDB | `cdb -server tcp:port=5005 -o out\Debug\<program>.exe` |
| Linux | GDB | `gdb ./out/<program>` |
| macOS | LLDB | `lldb ./out/<program>` |

## MCP Remote Debugging

If an MCP debugger server is configured, connect via natural language. Check the project's MCP configuration for available servers before attempting to connect.

| Platform | MCP Server |
|----------|------------|
| Windows | mcp-windbg — connect with `tcp:Port=5005,Server=localhost` |
| Linux | mcp-gdb — `uvx mcp-gdb` |
| macOS | lldb-mcp — `git clone https://github.com/stass/lldb-mcp` |

## Common Pitfalls

| Issue | Cause | Fix |
|-------|-------|-----|
| Crash only in Release | Uninitialized variable (zeroed in Debug, garbage in Release) | Run ASAN in Debug, inspect the variable |
| Works on Linux, crashes on Windows | `long` is 4 bytes on Windows x64, 8 on Linux | Use `int32_t`/`int64_t` instead of `long` |
| Heap corruption | Buffer overflow or use-after-free | Run ASAN |
| Data race | Missing lock or atomic | Run TSAN |
| Segfault with no ASAN output | Stack overflow (default 1MB on Windows, 8MB on Linux) | Reduce recursion or increase stack size |
| Core dump not generated (Linux) | `ulimit -c` is 0 | `ulimit -c unlimited` before running |
