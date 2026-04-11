---
name: c-project-build
description: >
  Use when building, compiling, testing, or running coverage for a C project,
  or when build fails, sanitizer reports errors, or coverage needs generating.
---

# C Project Build

## Overview

Build, test, and generate coverage for C/C++ CMake projects. Uses Ninja generator on all platforms for `compile_commands.json` support.

## When to Use

- Configuring or building a CMake project
- Running tests, sanitizers (ASAN, TSAN, UBSAN), or coverage

**When NOT to Use:** code style review, scaffolding new projects, committing code

## STOP — Required Before ANY Build Command

You MUST ask the user before running any cmake command:

1. Read `project(...)` from root `CMakeLists.txt` to get the project name
2. **Ask the user** which build type they want. Present all four CMake options:
   - **Debug** — no optimization, with debug info
   - **Release** — speed optimization (`/O2` or `-O2`)
   - **RelWithDebInfo** — speed optimization + debug info
   - **MinSizeRel** — size optimization (`/O1` or `-Os`)
3. Scan `CMakeLists.txt` for `option(...)` lines and **ask the user** which features to enable
4. **Delete `out/` before configure** — always start fresh to prevent stale cache (e.g. TLS objects lingering after disabling TLS)
5. Only then proceed

**An existing `out/` does NOT exempt you.** If you catch yourself thinking "out/ exists, I'll just build" or "it was Debug last time" — STOP and ask.

## Quick Reference

| Task | Command |
|------|---------|
| Configure | `cmake -B out -G Ninja -DCMAKE_BUILD_TYPE={type}` |
| Build | `cmake --build out -j {ncpu}` |
| Test | `ctest --test-dir out --output-on-failure` |
| Coverage | `cmake --build out --target coverage` |

See [build.md](references/build.md) for platform-specific flags, sanitizers, and full details.

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
