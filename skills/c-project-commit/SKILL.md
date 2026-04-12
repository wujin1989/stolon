---
name: c-project-commit
description: >
  Use when staging, committing, or pushing code changes in a C project.
---

# C Project Commit

## When to Use

- User asks to commit, push, or check in code

**When NOT to Use:** building/testing only, writing code, fixing build errors

## STOP — Required Before ANY Git Command

**MANDATORY — locate and read the commit reference before any git commit command:**

Run a shell command to find `c-project-commit/references/commit.md`. Search the project root first, then the user home directory. Use the platform-appropriate command:

**Unix:**
```
f=$(find . ~ -maxdepth 6 -path "*/c-project-commit/references/commit.md" -print -quit 2>/dev/null) && echo "$f"
```

**Windows (PowerShell):**
```
@('.', $HOME) | ForEach-Object { Get-ChildItem -Path $_ -Recurse -Depth 5 -Filter 'commit.md' -ErrorAction SilentlyContinue } | Where-Object { $_.FullName -match 'c-project-commit[\\/]references[\\/]commit\.md$' } | Select-Object -First 1 -ExpandProperty FullName
```

Call `readFile` on the result. If not found, STOP and tell the user.

## Red Flags

- Running `git commit` without having read commit.md this session
- Using past tense ("added" instead of "add")
- Missing scope (`feat:` instead of `feat(tcp):`)
- Running `git add .` without reviewing untracked files
