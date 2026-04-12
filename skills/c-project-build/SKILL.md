---
name: c-project-build
description: >
  Use when building, compiling, testing, or running coverage for a C project,
  or when build fails, sanitizer reports errors, or coverage needs generating.
---

# C Project Build

## When NOT to Use

code style review → c-project-style, scaffolding → c-project-init, committing → c-project-commit

## STOP — Read Reference Before ANY Build Command

Locate and read `c-project-build/references/build.md` under the current working directory and `$HOME`:

**Unix:**
```
find . ~ -maxdepth 6 -path "*/c-project-build/references/build.md" -print -quit 2>/dev/null
```

**Windows (PowerShell):**
```
@('.', $HOME) | % { gci $_ -R -Depth 5 -Filter build.md -EA 0 } | ? { $_.FullName -match 'c-project-build[\\/]references[\\/]build\.md$' } | select -First 1 -Exp FullName
```

Call `readFile` on the result. If not found, STOP and tell the user.

Follow the **Inputs — MANDATORY Pre-Flight** section in `build.md` before running any cmake command. No exceptions.

**Rebuild only?** If the user explicitly says they only changed `.c`/`.h` files AND you have completed a full configure in this session, skip confirmation and run `cmake --build out --config {build_type}` directly.
