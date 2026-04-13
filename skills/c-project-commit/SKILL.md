---
name: c-project-commit
description: >
  Use when staging, committing, or pushing code changes in a C project.
---

# C Project Commit

## When NOT to Use

building/testing → c-project-build, writing code → c-project-style

## STOP — Read Reference Before ANY Git Command

Locate and read `c-project-commit/references/commit.md` under the current working directory and `$HOME`:

**Unix:**
```
find . ~ -maxdepth 6 -path "*/c-project-commit/references/commit.md" -print -quit 2>/dev/null
```

**Windows (PowerShell):**
```
@('.', $HOME) | % { gci $_ -R -Depth 5 -Filter commit.md -EA 0 } | ? { $_.FullName -match 'c-project-commit[\\/]references[\\/]commit\.md$' } | select -First 1 -Exp FullName
```

**Windows (cmd):**
```
where /R . commit.md 2>nul & where /R "%USERPROFILE%" commit.md 2>nul | findstr /I "c-project-commit\\references\\commit.md"
```

Call `readFile` on the result. If not found, STOP and tell the user.
