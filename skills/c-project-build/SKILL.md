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

You MUST do these before running any cmake command:

1. Read `project(...)` from root `CMakeLists.txt` to get the project name
2. **Ask the user** which build type: Debug / Release / RelWithDebInfo / MinSizeRel
3. Scan `CMakeLists.txt` for `option(...)` lines and **ask the user** which features to enable
4. **Delete `out/` before configure** — always start fresh to prevent stale cache
5. Only then proceed

**An existing `out/` does NOT exempt you.**

**Rebuild only?** If the user explicitly says they only changed `.c`/`.h` files AND you have completed a full configure in this session with confirmed-correct settings, you may skip confirmation and run `cmake --build out` directly. If you have NOT configured in this session, you MUST run the full flow.

## STOP — You MUST Read build.md Before Running ANY Command

After collecting inputs, you MUST read this skill's `references/build.md` before executing. Do NOT use the commands below as-is — they are orientation only, not executable recipes.

**How to locate build.md:** The file `references/build.md` is located relative to this skill's SKILL.md. Use the following search strategy in order — stop at the first hit:

1. `fileSearch` for `c-project-build/references/build.md`
2. If step 1 returns nothing, `fileSearch` for `references/build.md` and pick the result whose path contains `c-project-build`

Read the file at whichever path you find first. Do NOT guess the path. Do NOT read any other file with a similar name in the project tree. If NONE of the above steps find the file, **STOP and tell the user** the skill reference is missing — do NOT fall back to any other file.

build.md contains critical details you cannot infer: platform-specific flags, sanitizer cmake options, compile_commands.json handling, Windows MSVC environment setup, coverage workflows, and CPU detection.

**Running commands without reading build.md WILL produce broken builds.**

**Do NOT fall back to general CMake knowledge or the project's own docs/build.md.** This skill uses Ninja Multi-Config on all platforms with build type selected at build time via `--config` (NOT `-DCMAKE_BUILD_TYPE`). General CMake patterns WILL produce broken builds.

| Excuse | Reality |
|--------|---------|
| "I know how CMake works" | This project uses Ninja Multi-Config, not single-config Ninja — the build type mechanism is different |
| "I'll just use cmake -DCMAKE_BUILD_TYPE=Debug" | WRONG. Multi-config generators ignore this flag. Build type is `--config Debug` at build time |
| "The user needs a quick build" | A broken build wastes MORE time than waiting |
| "The project has its own build docs" | That's a different file. This skill's build.md has Ninja Multi-Config specifics you won't find elsewhere |
| "I already read it last session" | Skill context resets each session. Read it again. |

## Red Flags — STOP and Re-read build.md

- About to run `cmake` without having read build.md this session
- Using `-DCMAKE_BUILD_TYPE=` anywhere (Multi-Config uses `--config` at build time)
- Skipping `out/` deletion "because config hasn't changed"
- Guessing platform flags instead of reading build.md
- Using the project's own build documentation instead of this skill's references/build.md

**Any of these mean: STOP. Read build.md. Then proceed.**

## Common Mistakes

- Forgetting to delete `out/` before every configure
- Enabling ASAN and TSAN simultaneously (they conflict)
- Not copying `compile_commands.json` to project root after configure
- Missing MSVC environment on Windows with Ninja
- Assuming Windows lacks coverage — check tests/CMakeLists.txt

## Workflow Routing — You MUST Read ALL Referenced Files

| Intent | Reference |
|--------|-----------|
| Build, test, sanitizers, or coverage | [build.md](references/build.md) |

**You MUST read every file listed above before executing.** No exceptions.
