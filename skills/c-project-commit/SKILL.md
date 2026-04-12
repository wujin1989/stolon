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

Search for `c-project-commit/references/commit.md` under the current working directory and `$HOME`. Use the platform-appropriate command:

**Unix:**
```
find . ~ -maxdepth 6 -path "*/c-project-commit/references/commit.md" -print -quit 2>/dev/null
```

**Windows (PowerShell):**
```
@('.', $HOME) | % { gci $_ -R -Depth 5 -Filter commit.md -EA 0 } | ? { $_.FullName -match 'c-project-commit[\\/]references[\\/]commit\.md$' } | select -First 1 -Exp FullName
```

Call `readFile` on the result. If not found, STOP and tell the user.

## Red Flags

- Running `git commit` without having read commit.md this session
- Using past tense ("added" instead of "add")
- Missing scope (`feat:` instead of `feat(tcp):`)
- Running `git add .` without reviewing untracked files
