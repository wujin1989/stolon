---
name: c-project-debug
description: >
  Use when a C executable crashes, segfaults, aborts, hangs, or produces
  unexpected behavior. Use when investigating SIGSEGV, SIGABRT, use-after-free,
  double-free, null pointer dereference, or memory corruption in a CMake/Ninja
  C project.
---

# C Project Debug

## When NOT to Use

build failures → c-project-build, code style → c-project-style

## STOP — Read Reference Before ANY Debug Action

Read `references/debug.md` in this skill's base directory. If not found, STOP and tell the user.

Follow the three-tier strategy in `debug.md` (reproduce → sanitizers → debugger). No shortcuts.
