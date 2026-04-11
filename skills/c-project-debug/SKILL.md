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
- User reports "getting a segfault" or "program crashes"

**When NOT to Use:** build failures (use c-project-build), code style issues (use c-project-style)

## STOP — Sanitizers Before Debugger

Before launching ANY debugger (gdb, lldb, cdb), you MUST:

1. Rebuild with ASAN (+ UBSAN on Unix)
2. Run the failing test
3. Read the sanitizer output

Only proceed to debugger if sanitizer output doesn't identify the bug or you need variable values at crash time.

| Excuse | Reality |
|--------|---------|
| "User already tried GDB" | ASAN output is clearer. 2 min rebuild saves 20 min guessing. |
| "Rebuilding with ASAN is slow" | Same build time. You're just adding a flag. |
| "I know it's a null pointer" | ASAN will confirm AND show the allocation site. Try first. |
| "Bug doesn't repro under ASAN" | Then say so and proceed to debugger. But TRY first. |

## Strategy (All Platforms)

```
1. Sanitizer rebuild (ASAN/UBSAN) — best diagnostics, always try first
2. Batch debugger (cdb on Windows, gdb/lldb on Unix) — if sanitizer isn't enough
3. Log-based debugging — fallback when no debugger is installed
```

## Quick Reference

| Step | Command | Purpose |
|------|---------|---------|
| Identify failing test | `ctest --test-dir out -R {module} --output-on-failure` | See which test function aborts/crashes |
| ASAN rebuild | Reconfigure with `-D{NAME}_ENABLE_ASAN=ON`, rebuild, run | Best crash diagnostics |
| Backtrace (Unix) | `gdb -batch -ex run -ex "bt full" out/test-{module}` | Crash location + locals |
| Backtrace (Windows) | `cdb -g -G -c "kb;q" out\test-{module}.exe` | Crash location + stack |
| Fallback | Add `fprintf(stderr, ...)` at key points, rebuild, run | When no debugger available |

## Workflow

See [debug.md](references/debug.md) for the complete step-by-step workflow, debugger commands, sanitizer usage, installation guides, and log-based debugging patterns.

## Common Mistakes

- Jumping to debugger before trying sanitizers — ASAN output is usually clearer
- Forgetting Debug build type (`-g -O0`) — always rebuild in Debug before debugging
- Running GDB on Windows or CDB on Unix — use the platform's native debugger
- Setting too many breakpoints in batch mode — keep it focused
- Adding logs without rebuilding — C requires recompilation

## Workflow Routing

| Intent | Reference |
|--------|-----------|
| Debug a crash or unexpected behavior | [debug.md](references/debug.md) |
