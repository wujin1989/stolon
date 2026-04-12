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

Run a shell command to find `c-project-style/references/style.md`. Search the project root first, then the user home directory. Use the platform-appropriate command:

**Unix:**
```
f=$(find . ~ -maxdepth 6 -path "*/c-project-style/references/style.md" -print -quit 2>/dev/null) && echo "$f"
```

**Windows (PowerShell):**
```
@('.', $HOME) | ForEach-Object { Get-ChildItem -Path $_ -Recurse -Depth 5 -Filter 'style.md' -ErrorAction SilentlyContinue } | Where-Object { $_.FullName -match 'c-project-style[\\/]references[\\/]style\.md$' } | Select-Object -First 1 -ExpandProperty FullName
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
