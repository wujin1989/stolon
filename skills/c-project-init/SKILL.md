---
name: c-project-init
description: >
  Use when creating a new C project from scratch, or when asked to scaffold,
  init, or bootstrap a C library or application project with CMake.
---

# C Project Init

## When to Use

- User asks to create, scaffold, init, or bootstrap a C project

**When NOT to Use:** building existing projects, writing code in existing projects, committing

## STOP — Required Before Generating ANY Files

**MANDATORY — locate and read the setup reference before writing a single file:**

Search for `c-project-init/references/setup.md` under the current working directory and `$HOME`. Use the platform-appropriate command:

**Unix:**
```
find . ~ -maxdepth 6 -path "*/c-project-init/references/setup.md" -print -quit 2>/dev/null
```

**Windows (PowerShell):**
```
@('.', $HOME) | % { gci $_ -R -Depth 5 -Filter setup.md -EA 0 } | ? { $_.FullName -match 'c-project-init[\\/]references[\\/]setup\.md$' } | select -First 1 -Exp FullName
```

Call `readFile` on the result. If not found, STOP and tell the user.

Collect ALL inputs listed in `setup.md`'s Inputs table before generating anything. Do NOT assume defaults.

## Red Flags

- About to generate files without having read setup.md this session
- setup.md read failed → STOP, do not generate
- User gave partial inputs → STILL ASK for the rest before generating
