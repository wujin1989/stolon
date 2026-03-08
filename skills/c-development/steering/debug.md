# Debugging

> Recommended trigger: manual include. Use when debugging issues.

## Diagnosis Order

Follow this order before reaching for a debugger:

1. Read the error message — compiler warnings, sanitizer output, assertion failures
2. Reproduce with a minimal test case (`tests/test-<module>.c`)
3. Run with sanitizers (ASAN → TSAN → UBSAN) to catch memory/thread/UB issues
4. Use a debugger only if sanitizers don't reveal the cause

## Sanitizer Constraints

Sanitizer build commands are in `build.md`. Additional constraints for debugging:

- ASAN and TSAN cannot be enabled simultaneously
- Always build in Debug mode for full symbol info (`-C Debug` on Windows)
- ASAN is available on all platforms (MSVC, GCC, Clang). TSAN and UBSAN are GCC/Clang only.

## Common Pitfalls

| Issue | Cause | Fix |
|-------|-------|-----|
| Crash only in Release | Uninitialized variable (zeroed in Debug, garbage in Release) | Run ASAN in Debug, inspect the variable |
| Works on Linux, crashes on Windows | `long` is 4 bytes on Windows x64, 8 on Linux | Use `int32_t`/`int64_t` instead of `long` |
| Heap corruption | Buffer overflow or use-after-free | Run ASAN |
| Data race | Missing lock or atomic | Run TSAN |
| Segfault with no ASAN output | Stack overflow (default 1MB on Windows, 8MB on Linux) | Reduce recursion or increase stack size |
| Core dump not generated (Linux) | `ulimit -c` is 0 | `ulimit -c unlimited` before running |
