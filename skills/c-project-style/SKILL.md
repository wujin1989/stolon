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

**MANDATORY — locate and read the style reference before generating or modifying any `.c` or `.h` file:**

Search for `c-project-style/references/style.md` under the current working directory and `$HOME`. Use the platform-appropriate command:

**Unix:**
```
find . ~ -maxdepth 6 -path "*/c-project-style/references/style.md" -print -quit 2>/dev/null
```

**Windows (PowerShell):**
```
@('.', $HOME) | % { gci $_ -R -Depth 5 -Filter style.md -EA 0 } | ? { $_.FullName -match 'c-project-style[\\/]references[\\/]style\.md$' } | select -First 1 -Exp FullName
```

Call `readFile` on the result. If not found, STOP and tell the user.

## Red Flags

- Writing `//` comments in `.c` or `.h` files
- Using `#pragma once` or `#ifndef` include guards
- Static functions with `<project>_` prefix (should be `_<module>_`)
- Using `int`/`unsigned` instead of fixed-width types
- Platform-specific code (`#ifdef _WIN32`) outside `src/platform/`
- Using `sprintf`, `strcpy`, or other restricted functions
- Missing license header block at top of file
