---
name: c-project-commit
description: >
  Use when staging, committing, or pushing code changes in a C project.
---

# C Project Commit

## Overview

Stage, commit, and push code changes with pre-commit validation (build + test + sanitizer) and conventional commit message formatting.

## When to Use

- User asks to commit, push, or check in code

**When NOT to Use:** building/testing only, writing code, fixing build errors

## STOP — Required Before ANY Commit

You MUST run the full pre-commit checklist before `git commit`:

1. Build (Debug) + test
2. Clean rebuild with ASAN + UBSAN, run tests again
3. Review diff, stage, commit, push

**Do NOT skip sanitizer even if tests pass.** "Tests passed, sanitizer is overkill" — STOP and run it.

## Quick Reference

| Step | Action |
|------|--------|
| Build + test | `cmake -B out ... -D{NAME}_ENABLE_TESTING=ON` then `ctest` |
| Sanitizer | Clean rebuild with ASAN + UBSAN, run tests |
| Stage | `git add -u` (review untracked before `git add .`) |
| Commit | `git commit -m "<type>(<scope>): <summary>"` |
| Push | `git push origin <branch>` |

Types: `feat`, `fix`, `refactor`, `test`, `docs`, `chore`, `perf`

## Common Mistakes

- Skipping sanitizer check before commit
- Using `git add .` without reviewing untracked files
- Past tense in commit messages ("added" instead of "add")
- Missing scope (e.g. `feat: add timeout` → `feat(tcp): add timeout`)

## Workflow Routing

| Intent | Reference |
|--------|-----------|
| Commit and push code changes | [commit.md](references/commit.md) |
