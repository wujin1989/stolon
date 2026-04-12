---
name: c-project-build
description: >
  Use when building, compiling, testing, or running coverage for a C project,
  or when build fails, sanitizer reports errors, or coverage needs generating.
---

# C Project Build

## Overview

Every build command requires reading build.md first — no exceptions. Covers C/C++ CMake projects using Ninja Multi-Config on all platforms.

## When to Use

- Configuring or building a CMake project
- Running tests, sanitizers (ASAN, TSAN, UBSAN), or coverage

**When NOT to Use:** code style review, scaffolding new projects, committing code

## STOP — Required Before ANY Build Command

1. Read `project(...)` from root `CMakeLists.txt` to get the project name
2. **Ask the user** which build type: Debug / Release / RelWithDebInfo / MinSizeRel
3. Scan `CMakeLists.txt` for `option(...)` lines and **ask the user** which features to enable
4. **Delete `out/` before configure** — always start fresh to prevent stale cache
5. Only then proceed

**Rebuild only?** If the user explicitly says they only changed `.c`/`.h` files AND you have completed a full configure in this session, you may skip confirmation and run `cmake --build out` directly. Otherwise, run the full flow.

## STOP — Read build.md Before Running ANY Command

Read this skill's `references/build.md` before executing. Commands below are orientation only, not executable recipes.

**How to locate:** Read `references/build.md` relative to the directory containing this `SKILL.md`. Derive the path from where you loaded this file. Do NOT guess. Do NOT use `fileSearch`.

**If not found, STOP. Tell the user the reference is missing. Do NOT proceed.**

This project uses Ninja Multi-Config — `CMAKE_BUILD_TYPE` is ignored, build type is `--config <type>` at build time. General CMake knowledge will produce broken builds.

## Red Flags — STOP and Re-read build.md

- About to run `cmake` without having read build.md this session
- Using `-DCMAKE_BUILD_TYPE=` anywhere
- Skipping `out/` deletion
- Guessing platform flags instead of reading build.md
- Using the project's own build docs instead of this skill's references/build.md

## Common Mistakes

- Forgetting to delete `out/` before every configure
- Enabling ASAN and TSAN simultaneously (they conflict)
- Not copying `compile_commands.json` to project root after configure
- Missing MSVC environment on Windows with Ninja

## Reference

| Intent | File |
|--------|------|
| Build, test, sanitizers, or coverage | [build.md](references/build.md) |
