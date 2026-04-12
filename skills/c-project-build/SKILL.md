---
name: c-project-build
description: >
  Use when building, compiling, testing, or running coverage for a C project,
  or when build fails, sanitizer reports errors, or coverage needs generating.
---

# C Project Build

## When to Use

- Configuring or building a CMake project
- Running tests, sanitizers (ASAN, TSAN, UBSAN), or coverage

**When NOT to Use:** code style review, scaffolding new projects, committing code

## STOP — Required Before ANY Build Command

**MANDATORY — locate and read the build reference before any cmake command:**

Run a shell command to find `c-project-build/references/build.md`. Search the project root first, then the user home directory. Use the platform-appropriate command:

**Unix:**
```
f=$(find . ~ -maxdepth 6 -path "*/c-project-build/references/build.md" -print -quit 2>/dev/null) && echo "$f"
```

**Windows (PowerShell):**
```
@('.', $HOME) | ForEach-Object { Get-ChildItem -Path $_ -Recurse -Depth 5 -Filter 'build.md' -ErrorAction SilentlyContinue } | Where-Object { $_.FullName -match 'c-project-build[\\/]references[\\/]build\.md$' } | Select-Object -First 1 -ExpandProperty FullName
```

Call `readFile` on the result. If not found, STOP and tell the user.

Follow the **Inputs — MANDATORY Pre-Flight** section in `build.md` before running any cmake command. No exceptions.

**Rebuild only?** If the user explicitly says they only changed `.c`/`.h` files AND you have completed a full configure in this session, skip confirmation and run `cmake --build out --config {build_type}` directly.

## Red Flags

- Running `cmake` without having read build.md this session
- Using `-DCMAKE_BUILD_TYPE=` (Ninja Multi-Config selects type at build time)
- Skipping `out/` deletion before configure
- Enabling ASAN and TSAN simultaneously
