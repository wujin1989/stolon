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

Run a shell command to find `c-project-init/references/setup.md`. Search the project root first, then the user home directory. Use the platform-appropriate command:

**Unix:**
```
f=$(find . ~ -maxdepth 6 -path "*/c-project-init/references/setup.md" -print -quit 2>/dev/null) && echo "$f"
```

**Windows (PowerShell):**
```
@('.', $HOME) | ForEach-Object { Get-ChildItem -Path $_ -Recurse -Depth 5 -Filter 'setup.md' -ErrorAction SilentlyContinue } | Where-Object { $_.FullName -match 'c-project-init[\\/]references[\\/]setup\.md$' } | Select-Object -First 1 -ExpandProperty FullName
```

Call `readFile` on the result. If not found, STOP and tell the user.

Collect ALL inputs listed in `setup.md`'s Inputs table before generating anything. Do NOT assume defaults.

## Red Flags

- About to generate files without having read setup.md this session
- setup.md read failed → STOP, do not generate
- User gave partial inputs → STILL ASK for the rest before generating
