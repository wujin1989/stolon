---
name: c-project-style
description: >
  Use when writing, reviewing, or modifying any .c or .h file in a C project.
  Use before creating new modules, refactoring existing code, or reviewing pull
  requests for style compliance.
---

# C Project Style

## Overview

Code style reference for C projects. All rules use `<project>` (lowercase from root `CMakeLists.txt`) and `<PROJECT>` (uppercase form) as placeholders.

## When to Use

- Writing, reviewing, or refactoring `.c` or `.h` files
- Adding modules, naming symbols, include ordering, memory management

**When NOT to Use:** build/CMake commands, scaffolding new projects, committing code

## STOP — Read style.md Before Writing or Reviewing ANY Code

Read this skill's `references/style.md` before generating or modifying any `.c` or `.h` file. Do NOT write code from memory or general C conventions.

**How to locate:** Read `references/style.md` relative to the directory containing this `SKILL.md`. Derive the path from where you loaded this file. Do NOT guess. Do NOT use `fileSearch`.

**If not found, STOP. Tell the user the reference is missing. Do NOT proceed.**

style.md contains 23 sections of project-specific rules that CANNOT be inferred from general C knowledge. This project uses `/* */` comments only, `_Pragma("once")`, fixed-width types, `init`/`deinit` vs `create`/`destroy` based on memory ownership, ASCII-only source files, and restricted functions.

## Red Flags — STOP and Re-read style.md

- About to write `//` comment in a `.c` or `.h` file
- About to use `#pragma once` or `#ifndef` include guard
- Naming a static function with `<project>_` prefix instead of `_<module>_`
- Using `int`/`unsigned` instead of fixed-width types
- Writing platform-specific code (`#ifdef _WIN32`) outside `src/platform/`
- About to use `sprintf`, `strcpy`, or other restricted functions (style.md §18)
- Missing license header block at top of file

## Common Mistakes

- Using `//` comments instead of `/* */`
- Missing `extern` on function declarations in headers
- Using `sprintf`/`strcpy` instead of `snprintf`
- `#ifndef` guards instead of `_Pragma("once")`
- Static functions with `<project>_` prefix (should be `_<module>_`)
- Non-ASCII characters in `.c`/`.h` files (ASCII only)
- Mixing `init`/`deinit` with `create`/`destroy`
- Using `int`/`unsigned` for struct fields instead of fixed-width types
- Not checking `calloc`/`malloc` return values

## Reference

| Intent | File |
|--------|------|
| Writing or reviewing C code | [style.md](references/style.md) |
