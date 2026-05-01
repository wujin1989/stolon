---
name: c-project-init
description: >
  Use when creating a new C project from scratch, or when asked to scaffold,
  init, or bootstrap a C library or application project with CMake. Also use
  when the user says "new project", "start a project", or needs a complete
  CMake + test + platform-layer skeleton generated.
---

# C Project Init

## When NOT to Use

building → c-project-build, writing code → c-project-style, committing → c-project-commit

## STOP — Read Reference Before Generating ANY Files

Read `references/setup.md` in this skill's base directory. If not found, STOP and tell the user.

Collect ALL inputs listed in `setup.md`'s Inputs table before generating anything. Do NOT assume defaults.
