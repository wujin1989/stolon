---
name: c-project-debug
description: >
  Use when a C executable crashes, segfaults, aborts, hangs, or produces
  unexpected behavior. Use when investigating SIGSEGV, SIGABRT, use-after-free,
  double-free, null pointer dereference, or memory corruption in a CMake/Ninja
  C project.
---

# C Project Debug

## When to Use

- Executable crashes (segfault, abort, bus error)
- Executable hangs or times out
- Sanitizer reports errors you need to investigate further

**When NOT to Use:** build failures (use c-project-build), code style issues (use c-project-style)

## STOP — Required Before ANY Debug Action

**MANDATORY — locate and read the debug reference before any debugger or sanitizer command:**

Run a shell command to find `c-project-debug/references/debug.md`. Search the project root first, then the user home directory. Use the platform-appropriate command:

**Unix:**
```
f=$(find . ~ -maxdepth 6 -path "*/c-project-debug/references/debug.md" -print -quit 2>/dev/null) && echo "$f"
```

**Windows (PowerShell):**
```
@('.', $HOME) | ForEach-Object { Get-ChildItem -Path $_ -Recurse -Depth 5 -Filter 'debug.md' -ErrorAction SilentlyContinue } | Where-Object { $_.FullName -match 'c-project-debug[\\/]references[\\/]debug\.md$' } | Select-Object -First 1 -ExpandProperty FullName
```

Call `readFile` on the result. If not found, STOP and tell the user.

Follow the three-tier strategy in `debug.md` (reproduce → sanitizers → debugger). No shortcuts.

## Red Flags

- Running `cmake` or debugger without having read debug.md this session
- Launching gdb/lldb/cdb without trying sanitizers first
- Running sanitizers without reproducing the failure first
- Running GDB on Windows or CDB on Unix
- Adding printf as first debugging step (try sanitizers first)
