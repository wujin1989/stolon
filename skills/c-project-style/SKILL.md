---
name: c-project-style
description: >
  Use when writing, reviewing, or modifying any .c or .h file in a C project.
  Use before creating new modules, refactoring existing code, or reviewing pull
  requests for style compliance.
---

# C Project Style

## When to Use

- Writing, reviewing, or refactoring `.c` or `.h` files
- Adding modules, naming symbols, include ordering, memory management

**When NOT to Use:** build/CMake commands, scaffolding new projects, committing code

## STOP — Required Before Writing or Reviewing ANY Code

**MANDATORY:** Read `references/style.md` before generating or modifying any `.c` or `.h` file.

1. **Project level:** Use `fileSearch` to search for `c-project-style/references/style.md` within the project being worked on.
2. **User level:** Search for `c-project-style/references/style.md` under the user home directory (`~` on Unix, `%USERPROFILE%` on Windows).

**If not found at either level, STOP. Tell the user the reference is missing. Do NOT proceed.**

## Red Flags

- Writing `//` comments in `.c` or `.h` files
- Using `#pragma once` or `#ifndef` include guards
- Static functions with `<project>_` prefix (should be `_<module>_`)
- Using `int`/`unsigned` instead of fixed-width types
- Platform-specific code (`#ifdef _WIN32`) outside `src/platform/`
- Using `sprintf`, `strcpy`, or other restricted functions
- Missing license header block at top of file
