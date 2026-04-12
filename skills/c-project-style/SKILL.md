---
name: c-project-style
description: >
  Use when writing, reviewing, or modifying any .c or .h file in a C project.
  Use before creating new modules, refactoring existing code, or reviewing pull
  requests for style compliance.
---

# C Project Style

## When NOT to Use

build/CMake → c-project-build, scaffolding → c-project-init, committing → c-project-commit

## STOP — Read Reference Before Writing or Reviewing ANY Code

Locate and read `c-project-style/references/style.md` under the current working directory and `$HOME`:

**Unix:**
```
find . ~ -maxdepth 6 -path "*/c-project-style/references/style.md" -print -quit 2>/dev/null
```

**Windows (PowerShell):**
```
@('.', $HOME) | % { gci $_ -R -Depth 5 -Filter style.md -EA 0 } | ? { $_.FullName -match 'c-project-style[\\/]references[\\/]style\.md$' } | select -First 1 -Exp FullName
```

Call `readFile` on the result. If not found, STOP and tell the user.
