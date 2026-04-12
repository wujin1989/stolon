---
name: c-project-build
description: >
  Use when building, compiling, testing, or running coverage for a C project,
  or when build fails, sanitizer reports errors, or coverage needs generating.
---

# C Project Build

## When to Use

- Configuring or building a CMake project
- Running tests, sanitizers (ASAN, TSAN, UBSAN), or coverage

**When NOT to Use:** code style review, scaffolding new projects, committing code

## STOP — Required Before ANY Build Command

**MANDATORY:** Read `references/build.md` before executing any build command.

1. **Project level:** Use `fileSearch` to search for `c-project-build/references/build.md` within the project being built.
2. **User level:** Search for `c-project-build/references/build.md` under the user home directory (`~` on Unix, `%USERPROFILE%` on Windows).

**If not found at either level, STOP. Tell the user the reference is missing. Do NOT proceed.**

Then, before configuring:

1. Read `project(...)` from root `CMakeLists.txt` to get the project name
2. **Ask the user** which build type: Debug / Release / RelWithDebInfo / MinSizeRel
3. Scan `CMakeLists.txt` for `option(...)` lines and **ask the user** which features to enable
4. **Delete `out/` before configure** — always start fresh to prevent stale cache

**Rebuild only?** If the user explicitly says they only changed `.c`/`.h` files AND you have completed a full configure in this session, skip confirmation and run `cmake --build out` directly.

## Red Flags

- Running `cmake` without having read build.md this session
- Using `-DCMAKE_BUILD_TYPE=`
- Skipping `out/` deletion before configure
- Enabling ASAN and TSAN simultaneously
