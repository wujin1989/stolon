---
name: c-project-build
description: >
  Use when building, compiling, testing, or running coverage for a C project,
  or when build fails, sanitizer reports errors, or coverage needs generating.
---

# C Project Build

## When NOT to Use

code style review → c-project-style, scaffolding → c-project-init, committing → c-project-commit

## STOP — Read Reference Before ANY Build Command

Read `references/build.md` in this skill's base directory. If not found, STOP and tell the user.

Follow the **Inputs — MANDATORY Checks Before Build** section in `build.md` before running any cmake command. No exceptions.

**Rebuild only?** If the user explicitly says they only changed `.c`/`.h` files AND you have completed a full configure in this session, skip confirmation and run `cmake --build out --config {build_type}` directly.
