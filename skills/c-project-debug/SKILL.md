---
name: c-project-debug
description: >
  Use when a C executable crashes, segfaults, aborts, hangs, or produces
  unexpected behavior. Use when investigating SIGSEGV, SIGABRT, use-after-free,
  double-free, null pointer dereference, or memory corruption in a CMake/Ninja
  C project.
---

# C Project Debug

## Overview

Non-interactive debugging workflow for C projects. Three-tier strategy: sanitizers first, then batch-mode debugger, then log-based debugging as fallback.

## When to Use

- Executable crashes (segfault, abort, bus error)
- Executable hangs or times out
- Sanitizer reports errors you need to investigate further

**When NOT to Use:** build failures (use c-project-build), code style issues (use c-project-style)

## STOP — Read debug.md Before ANY Debug Action

Read this skill's `references/debug.md` before running any debugger or sanitizer command. Do NOT use commands from memory.

**How to locate:** Read `references/debug.md` relative to the directory containing this `SKILL.md`. Derive the path from where you loaded this file. Do NOT guess. Do NOT use `fileSearch`.

**If not found, STOP. Tell the user the reference is missing. Do NOT proceed.**

debug.md contains the complete workflow, platform-specific debugger commands (cdb/gdb/lldb), sanitizer interpretation, hang debugging, and log-based debugging strategy. This project uses batch-mode debuggers (non-interactive) with specific flags — general debugging knowledge will miss the root cause.

## STOP — Reproduce First, Sanitizers Before Debugger

1. **Reproduce:** Run `ctest --test-dir out -C Debug -R {module} --output-on-failure` — note the signal and failing assertion
2. **Sanitizers:** Rebuild with ASAN (+ UBSAN on Unix), run the failing test, read output
3. **Debugger:** Only if sanitizer output doesn't identify the bug

Re-run ctest even if you saw output earlier — stale information is not a substitute for a fresh reproduce.

## Red Flags

- About to launch gdb/lldb/cdb without trying sanitizers first
- Running sanitizers without reproducing the failure first
- Running GDB on Windows or CDB on Unix
- Adding printf as first debugging step (try sanitizers first)

## Common Mistakes

- Jumping to debugger before trying sanitizers
- Forgetting Debug build type before debugging
- Running GDB on Windows or CDB on Unix
- Adding logs without rebuilding — C requires recompilation

## Reference

| Intent | File |
|--------|------|
| Debug a crash or unexpected behavior | [debug.md](references/debug.md) |
