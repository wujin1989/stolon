---
name: c-project-debug
description: >
  Use when a C executable crashes, segfaults, aborts, hangs, or produces
  unexpected behavior. Use when investigating SIGSEGV, SIGABRT, use-after-free,
  double-free, null pointer dereference, or memory corruption in a CMake/Ninja
  C project.
---

# C Project Debug

## When to Use

- Executable crashes (segfault, abort, bus error)
- Executable hangs or times out
- Sanitizer reports errors you need to investigate further

**When NOT to Use:** build failures (use c-project-build), code style issues (use c-project-style)

## STOP — Required Before ANY Debug Action

**MANDATORY:** Read `references/debug.md` before running any debugger or sanitizer command.

1. **Project level:** Use `fileSearch` to search for `c-project-debug/references/debug.md` within the project being debugged.
2. **User level:** Search for `c-project-debug/references/debug.md` under the user home directory (`~` on Unix, `%USERPROFILE%` on Windows).

**If not found at either level, STOP. Tell the user the reference is missing. Do NOT proceed.**

Then, before debugging:

1. **Reproduce:** Run `ctest --test-dir out -C Debug -R {module} --output-on-failure`
2. **Sanitizers:** Rebuild with ASAN (+ UBSAN on Unix), run the failing test
3. **Debugger:** Only if sanitizer output doesn't identify the bug

## Red Flags

- Launching gdb/lldb/cdb without trying sanitizers first
- Running sanitizers without reproducing the failure first
- Running GDB on Windows or CDB on Unix
- Adding printf as first debugging step (try sanitizers first)
