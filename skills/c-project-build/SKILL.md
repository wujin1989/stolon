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

**Skip confirmation when:**

1. **Rebuild only** — the user only changed `.c`/`.h` files AND a full configure was completed in this session. Run `cmake --build out --config {build_type}` directly.
2. **Re-run tests** — a build already succeeded in this session and the user wants to run tests again (e.g. after a code fix). Run `ctest --test-dir out -C {build_type} ...` directly.
3. **Iterative fix cycle** — the user is in a fix → rebuild → test loop within the same session. Only rebuild + test; do not re-ask build type or feature flags.
