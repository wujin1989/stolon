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

**MANDATORY:** Read `references/commit.md` before executing any git commit command.

1. **Project level:** Use `fileSearch` to search for `c-project-commit/references/commit.md` within the project being committed.
2. **User level:** Search for `c-project-commit/references/commit.md` under the user home directory (`~` on Unix, `%USERPROFILE%` on Windows).

**If not found at either level, STOP. Tell the user the reference is missing. Do NOT proceed.**

## Red Flags

- Running `git commit` without having read commit.md this session
- Using past tense ("added" instead of "add")
- Missing scope (`feat:` instead of `feat(tcp):`)
- Running `git add .` without reviewing untracked files
