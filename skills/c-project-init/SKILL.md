---
name: c-project-init
description: >
  Use when creating a new cross-platform C project from scratch, or when asked
  to scaffold, init, or bootstrap a C library or application project.
---

# C Project Init

## Overview

Scaffold a new cross-platform C11 project (library or application) with CMake, platform layer, test harness, and build scripts.

## When to Use

- User asks to create a new C project, library, or application
- Phrases like "new C project", "scaffold", "init", "bootstrap"
- Need a project skeleton with cross-platform support (Windows, Linux, macOS)

## When NOT to Use

- Project already exists and needs building
- Writing code in an existing project
- Committing or pushing changes

## Quick Reference

| Project type | What it generates |
|-------------|-------------------|
| Library | Static lib target, public headers in `include/`, umbrella header |
| Application | Executable target + internal `_lib` static target for test linking |

Both types include: CMakeLists.txt, platform layer skeleton, test harness with custom ASSERT macro, build scripts, .clang-format, .gitignore, LICENSE, AUTHORS.

## Common Mistakes

- Forgetting to ask user for project type (library vs application)
- Not reading the LICENSE file content for the license header template
- Creating test executables manually instead of using `<project>_add_test()`

## Workflow Routing

| Intent | Reference |
|--------|-----------|
| Create a new C project (library or application) | [setup.md](references/setup.md) |
