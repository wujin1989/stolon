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

You MUST read this skill's `references/style.md` before generating or modifying any `.c` or `.h` file. Do NOT write code from memory or general C conventions.

**How to locate style.md:** You MUST use fileSearch to find `c-project-style/references/style.md`, then read the file at the returned path. Do NOT guess the path. Do NOT read any other file with a similar name in the project tree.

style.md contains 23 sections of project-specific rules: file naming, symbol naming, lifecycle patterns, memory management, platform layer rules, test framework, module addition checklists, and more. These conventions CANNOT be inferred from general C knowledge.

**Writing code without reading style.md WILL produce non-compliant output.** The Common Mistakes below cover ~5% of the style surface area.

**Do NOT fall back to general C conventions.** This project has 23 sections of specific rules (lifecycle naming, opaque structs, ASCII-only comments, restricted functions, intrusive data structures, etc.) that you WILL get wrong without style.md.

| Excuse | Reality |
|--------|---------|
| "I know C coding style" | This project has `init`/`deinit` vs `create`/`destroy` rules based on memory ownership — you'll mix them up |
| "I'll follow common conventions" | Common conventions use `//` comments, `#pragma once`, `int` types — all WRONG for this project |
| "The user just needs a quick function" | A non-compliant function creates tech debt and fails code review |
| "I read style.md but this rule seems unnecessary" | Every rule exists because of past code review failures. Follow all of them. |
| "I'll fix the style later" | Non-compliant code in PR = blocked review. Get it right the first time. |

## Red Flags — STOP and Re-read style.md

- About to write `//` comment in a `.c` or `.h` file
- About to use `#pragma once` or `#ifndef` include guard
- Naming a static function with `<project>_` prefix instead of `_<module>_`
- Using `int`/`unsigned` instead of fixed-width types (`int32_t`, `uint32_t`)
- Writing platform-specific code (`#ifdef _WIN32`) outside `src/platform/`
- About to use `sprintf`, `strcpy`, or other restricted functions (see style.md §18)
- About to write `#include <assert.h>` instead of `#include "assert.h"`
- Missing license header block at top of file
- About to write or modify code without having read style.md this session

**Any of these mean: STOP. Re-read style.md. Then proceed.**

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

## Workflow Routing — You MUST Read ALL Referenced Files

| Intent | Reference |
|--------|-----------|
| Writing or reviewing C code | [style.md](references/style.md) |

**You MUST read every file listed above before executing.** No exceptions.
