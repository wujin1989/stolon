---
name: c-project-init
description: >
  Use when creating a new C project from scratch, or when asked to scaffold,
  init, or bootstrap a C library or application project.
---

# C Project Init

## Overview

Scaffold a new C project (library or application) with CMake, cross-platform support, test harness, and build scripts.

## When to Use

- User asks to create, scaffold, init, or bootstrap a C project

**When NOT to Use:** building existing projects, writing code in existing projects, committing

## STOP — Required Before Generating Any Files

You MUST ask the user for all required inputs first:

1. Project name
2. Project type (library or application)
3. Description, author, email

**Do NOT assume defaults or start generating files before asking.** See [setup.md](references/setup.md) for the full inputs table.

## Quick Reference

| Project type | What it generates |
|-------------|-------------------|
| Library | Static lib, public headers in `include/`, umbrella header |
| Application | Executable + internal `_lib` static target for test linking |

Both include: CMakeLists.txt, platform skeleton, test harness, .clang-format, .gitignore, LICENSE, AUTHORS.

## Common Mistakes

- Forgetting to ask user for project type (library vs application)
- Not reading LICENSE file for the license header template
- Creating test executables manually instead of using `<project>_add_test()`

## Workflow Routing

| Intent | Reference |
|--------|-----------|
| Create a new C project | [setup.md](references/setup.md) |
