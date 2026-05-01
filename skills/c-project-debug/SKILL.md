---
name: c-project-debug
description: >
  Use when a C executable crashes, segfaults, aborts, hangs, or produces
  unexpected behavior. Use when investigating SIGSEGV, SIGABRT, use-after-free,
  double-free, null pointer dereference, or memory corruption in a CMake/Ninja
  C project. Also use when a test fails unexpectedly, when you need to attach
  a debugger, or when diagnosing why a program produces wrong output.
---

# C Project Debug

## When NOT to Use

build failures → c-project-build, code style → c-project-style

## STOP — Read Reference Before ANY Debug Action

Read `references/debug.md` in this skill's base directory. If not found, STOP and tell the user.

Follow the three-tier strategy in `debug.md` (reproduce → sanitizers → debugger). No shortcuts.
