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

You MUST read this skill's `references/debug.md` before running any debugger or sanitizer command. Do NOT use commands from memory.

**How to locate debug.md:** The file `references/debug.md` is located relative to this skill's SKILL.md. Use `fileSearch` for `c-project-debug/references/debug.md` (covers workspace-level and global-level skills). Read the file at the returned path. Do NOT guess the path. Do NOT read any other file with a similar name in the project tree.

debug.md contains the complete step-by-step workflow, platform-specific debugger commands (cdb/gdb/lldb), sanitizer interpretation, hang debugging, log-based debugging strategy, and debugger installation guides. None of this is in this file.

**Debugging without reading debug.md WILL waste time and miss the root cause.**

**Do NOT fall back to general debugging knowledge.** This project uses batch-mode debuggers (non-interactive), platform-specific commands (cdb/gdb/lldb), and a specific 3-tier strategy that you WILL get wrong without debug.md.

| Excuse | Reality |
|--------|---------|
| "I know how to use gdb" | This project uses batch-mode commands, not interactive sessions — the flags are different |
| "I'll just run with ASAN" | The workflow requires Step 0 (reproduce) FIRST, then sanitizers — skipping Step 0 wastes time |
| "Debugging is debugging" | Platform-specific debugger selection, hang debugging, and log-based fallback all have specific procedures |
| "The crash is obvious, I don't need sanitizers" | Obvious crashes often have non-obvious root causes. ASAN finds the real bug, not the symptom |
| "I'll just add a printf and see" | C requires recompilation. Sanitizers give better output with zero code changes |

## STOP — Step 0: Reproduce First

Before ANY sanitizer or debugger, you MUST identify which test fails:

```
ctest --test-dir out -C Debug -R {module} --output-on-failure
```

Note the signal (SIGSEGV, SIGABRT, SIGBUS) and the failing assertion line. Only then proceed to sanitizers.

**Even if you already saw test output earlier in this session, you MUST re-run ctest to confirm the current state.** Stale information from earlier in the conversation is not a substitute for a fresh reproduce.

## Red Flags — STOP and Follow the 3-Tier Strategy

- About to launch gdb/lldb/cdb without trying sanitizers first
- About to run sanitizers without reproducing the failure first (Step 0)
- Debugging without having read debug.md this session
- Running GDB on Windows or CDB on Unix
- Adding printf/log statements as first debugging step (try sanitizers first)

**Any of these mean: STOP. Follow the workflow: reproduce → sanitizers → debugger → logs.**

## Common Mistakes

- Jumping to debugger before trying sanitizers — ASAN output is usually clearer
- Forgetting Debug build type — always rebuild in Debug before debugging
- Running GDB on Windows or CDB on Unix — use the platform's native debugger
- Adding logs without rebuilding — C requires recompilation


## Workflow Routing — You MUST Read ALL Referenced Files

| Intent | Reference |
|--------|-----------|
| Debug a crash or unexpected behavior | [debug.md](references/debug.md) |

**You MUST read every file listed above before executing.** No exceptions.
