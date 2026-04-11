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

## Quick Reference

| Topic | Section in style.md |
|-------|---------------------|
| Naming conventions | 7. Symbol Naming |
| File/directory layout | 6. File Naming |
| Memory management | 16. Allocation, NULL-safe destroy, deferred free |
| Error handling | 17. Return conventions, logging |
| Test code rules | 21. Framework, structure, naming |
| Adding modules | 22. Single-file, multi-file, executable |
| Platform layer | 12. platform/ directory rules |

## Common Mistakes

- Using `//` comments instead of `/* */`
- Missing `extern` on function declarations in headers
- Using `sprintf`/`strcpy` instead of `snprintf`
- Platform conditionals (`#ifdef _WIN32`) outside `src/platform/`
- `#ifndef` guards instead of `_Pragma("once")`
- Static functions with `<project>_` prefix (should be `_<module>_`)

## Workflow Routing

| Intent | Reference |
|--------|-----------|
| Writing or reviewing C code | [style.md](references/style.md) |
