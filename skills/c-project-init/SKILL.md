---
name: c-project-init
description: >
  Use when creating a new C project from scratch, or when asked to scaffold,
  init, or bootstrap a C library or application project with CMake.
---

# C Project Init

## When NOT to Use

building → c-project-build, writing code → c-project-style, committing → c-project-commit

## STOP — Read Reference Before Generating ANY Files

Locate and read `c-project-init/references/setup.md` under the current working directory and `$HOME`:

**Unix:**
```
find . ~ -maxdepth 6 -path "*/c-project-init/references/setup.md" -print -quit 2>/dev/null
```

**Windows (PowerShell):**
```
@('.', $HOME) | % { gci $_ -R -Depth 5 -Filter setup.md -EA 0 } | ? { $_.FullName -match 'c-project-init[\\/]references[\\/]setup\.md$' } | select -First 1 -Exp FullName
```

**Windows (cmd):**
```
where /R . setup.md 2>nul & where /R "%USERPROFILE%" setup.md 2>nul | findstr /I "c-project-init\\references\\setup.md"
```

Call `readFile` on the result. If not found, STOP and tell the user.

Collect ALL inputs listed in `setup.md`'s Inputs table before generating anything. Do NOT assume defaults.
