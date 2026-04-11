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

### If style.md Cannot Be Read — STOP COMPLETELY

If the file read fails (file not found, access denied, any error):

1. **Do NOT write or modify any `.c` or `.h` file.**
2. Tell the user: "I cannot proceed — style.md is required but could not be read. Please check the file exists at the expected path."
3. **Do NOT fall back to general C conventions.** This project has 23 sections of specific rules (lifecycle naming, opaque structs, ASCII-only comments, restricted functions, intrusive data structures, etc.) that you WILL get wrong without style.md.

| Excuse | Reality |
|--------|---------|
| "I know C coding style" | This project has `init`/`deinit` vs `create`/`destroy` rules based on memory ownership — you'll mix them up |
| "I'll follow common conventions" | Common conventions use `//` comments, `#pragma once`, `int` types — all WRONG for this project |
| "The user just needs a quick function" | A non-compliant function creates tech debt and fails code review |

## Common Mistakes

- Using `//` comments instead of `/* */`
- Missing `extern` on function declarations in headers
- Using `sprintf`/`strcpy` instead of `snprintf` (see full restricted functions list in style.md §18)
- Platform conditionals (`#ifdef _WIN32`) outside `src/platform/`
- `#ifndef` guards instead of `_Pragma("once")`
- Static functions with `<project>_` prefix (should be `_<module>_`)
- Non-ASCII characters in `.c`/`.h` files (em dashes, smart quotes, CJK text — ASCII only)
- Mixing `init`/`deinit` with `create`/`destroy` — memory ownership determines the pattern
- Using `int`/`unsigned` for struct fields instead of fixed-width types (`int32_t`, `uint32_t`)
- Forgetting module addition checklist steps (CMakeLists.txt, umbrella header, test file, test registration)
- Using `#include <assert.h>` instead of `#include "assert.h"` (project macro)
- Not checking `calloc`/`malloc` return values

## Workflow Routing

| Intent | Reference |
|--------|-----------|
| Writing or reviewing C code | [style.md](references/style.md) |
