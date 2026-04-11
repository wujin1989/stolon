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

### If debug.md Cannot Be Read — STOP COMPLETELY

If the file read fails (file not found, access denied, any error):

1. **Do NOT run any debugger or sanitizer command.**
2. Tell the user: "I cannot proceed — debug.md is required but could not be read. Please check the file exists at the expected path."
3. **Do NOT fall back to general debugging knowledge.** This project uses batch-mode debuggers (non-interactive), platform-specific commands (cdb/gdb/lldb), and a specific 3-tier strategy that you WILL get wrong without debug.md.

| Excuse | Reality |
|--------|---------|
| "I know how to use gdb" | This project uses batch-mode commands, not interactive sessions — the flags are different |
| "I'll just run with ASAN" | The workflow requires Step 0 (reproduce) FIRST, then sanitizers — skipping Step 0 wastes time |
| "Debugging is debugging" | Platform-specific debugger selection, hang debugging, and log-based fallback all have specific procedures |

## STOP — Step 0: Reproduce First

Before ANY sanitizer or debugger, you MUST identify which test fails:

```
ctest --test-dir out -C Debug -R {module} --output-on-failure
```

Note the signal (SIGSEGV, SIGABRT, SIGBUS) and the failing assertion line. Only then proceed to sanitizers.

## Common Mistakes

- Jumping to debugger before trying sanitizers — ASAN output is usually clearer
- Forgetting Debug build type — always rebuild in Debug before debugging
- Running GDB on Windows or CDB on Unix — use the platform's native debugger
- Adding logs without rebuilding — C requires recompilation

## Workflow Routing

| Intent | Reference |
|--------|-----------|
| Debug a crash or unexpected behavior | [debug.md](references/debug.md) |
