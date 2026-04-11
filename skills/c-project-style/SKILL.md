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

## STOP — You MUST Read style.md Before Writing or Reviewing ANY Code

You MUST read [style.md](references/style.md) before generating or modifying any `.c` or `.h` file. Do NOT write code from memory or general C conventions.

style.md contains 23 sections of project-specific rules: file naming, symbol naming, lifecycle patterns, memory management, platform layer rules, test framework, module addition checklists, and more. These conventions CANNOT be inferred from general C knowledge.

**Writing code without reading style.md WILL produce non-compliant output.** The Common Mistakes below cover ~5% of the style surface area.

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
