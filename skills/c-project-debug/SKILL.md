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

## STOP — Sanitizers Before Debugger

Before launching ANY debugger (gdb, lldb, cdb), you MUST:

1. Rebuild with ASAN (+ UBSAN on Unix)
2. Run the failing test
3. Read the sanitizer output

Only proceed to debugger if sanitizer output doesn't identify the bug.

## STOP — You MUST Read debug.md Before ANY Debug Action

You MUST read [debug.md](references/debug.md) before running any debugger or sanitizer command. Do NOT use commands from memory.

debug.md contains the complete step-by-step workflow, platform-specific debugger commands (cdb/gdb/lldb), sanitizer interpretation, hang debugging, log-based debugging strategy, and debugger installation guides. None of this is in this file.

**Debugging without reading debug.md WILL waste time and miss the root cause.**

## Common Mistakes

- Jumping to debugger before trying sanitizers — ASAN output is usually clearer
- Forgetting Debug build type — always rebuild in Debug before debugging
- Running GDB on Windows or CDB on Unix — use the platform's native debugger
- Adding logs without rebuilding — C requires recompilation

## Workflow Routing

| Intent | Reference |
|--------|-----------|
| Debug a crash or unexpected behavior | [debug.md](references/debug.md) |
