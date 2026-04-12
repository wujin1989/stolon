---
name: c-project-debug
description: >
  Use when a C executable crashes, segfaults, aborts, hangs, or produces
  unexpected behavior. Use when investigating SIGSEGV, SIGABRT, use-after-free,
  double-free, null pointer dereference, or memory corruption in a CMake/Ninja
  C project.
---

# C Project Debug

## When NOT to Use

build failures → c-project-build, code style → c-project-style

## STOP — Read Reference Before ANY Debug Action

Locate and read `c-project-debug/references/debug.md` under the current working directory and `$HOME`:

**Unix:**
```
find . ~ -maxdepth 6 -path "*/c-project-debug/references/debug.md" -print -quit 2>/dev/null
```

**Windows (PowerShell):**
```
@('.', $HOME) | % { gci $_ -R -Depth 5 -Filter debug.md -EA 0 } | ? { $_.FullName -match 'c-project-debug[\\/]references[\\/]debug\.md$' } | select -First 1 -Exp FullName
```

Call `readFile` on the result. If not found, STOP and tell the user.

Follow the three-tier strategy in `debug.md` (reproduce → sanitizers → debugger). No shortcuts.
