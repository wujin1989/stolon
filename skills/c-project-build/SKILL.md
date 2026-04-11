---
name: c-project-build
description: >
  Use when building, compiling, testing, or running coverage for a C project,
  or when build fails, sanitizer reports errors, or coverage needs generating.
---

# C Project Build

## Overview

Build, test, and generate coverage for C/C++ CMake projects. Uses Ninja Multi-Config generator on all platforms.

## When to Use

- Configuring or building a CMake project
- Running tests, sanitizers (ASAN, TSAN, UBSAN), or coverage

**When NOT to Use:** code style review, scaffolding new projects, committing code

## STOP — Required Before ANY Build Command

You MUST do these before running any cmake command:

1. Read `project(...)` from root `CMakeLists.txt` to get the project name
2. **Ask the user** which build type: Debug / Release / RelWithDebInfo / MinSizeRel
3. Scan `CMakeLists.txt` for `option(...)` lines and **ask the user** which features to enable
4. **Delete `out/` before configure** — always start fresh to prevent stale cache
5. Only then proceed

**An existing `out/` does NOT exempt you.**

**Rebuild only?** If the user explicitly says they only changed `.c`/`.h` files and the current config is confirmed correct, you may skip confirmation and run `cmake --build out` directly.

## STOP — You MUST Read build.md Before Running ANY Command

After collecting inputs, you MUST read [build.md](references/build.md) before executing. Do NOT use the commands below as-is — they are orientation only, not executable recipes.

build.md contains critical details you cannot infer: platform-specific flags, sanitizer cmake options, compile_commands.json handling, Windows MSVC environment setup, coverage workflows, and CPU detection.

**Running commands without reading build.md WILL produce broken builds.**

## Common Mistakes

- Forgetting to delete `out/` before every configure
- Enabling ASAN and TSAN simultaneously (they conflict)
- Not copying `compile_commands.json` to project root after configure
- Missing MSVC environment on Windows with Ninja
- Assuming Windows lacks coverage — check tests/CMakeLists.txt

## Workflow Routing

| Intent | Reference |
|--------|-----------|
| Build, test, sanitizers, or coverage | [build.md](references/build.md) |
