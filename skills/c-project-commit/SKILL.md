---
name: c-project-commit
description: >
  Use when staging, committing, or pushing code changes in a C project.
  Use when asked to commit, push, submit code, or check in changes.
---

# C Project Commit

## Overview

Stage, commit, and push code changes with pre-commit validation (build + test + sanitizer) and conventional commit message formatting.

## When to Use

- User asks to commit, push, submit, or check in code
- Implementation is complete and needs to be committed
- Need to verify code before pushing

## When NOT to Use

- Code still has build errors or test failures (fix first)
- Only need to build or test, not commit
- Writing or reviewing code style

## Quick Reference

| Step | Action |
|------|--------|
| 1. Build | `cmake -B out -G Ninja -DCMAKE_BUILD_TYPE=Debug -D{NAME}_ENABLE_TESTING=ON` + build |
| 2. Test | `ctest --test-dir out --output-on-failure` |
| 3. Sanitizer | Clean rebuild with ASAN + UBSAN, run tests |
| 4. Review | `git diff` + `git status` |
| 5. Stage | `git add -u` (review untracked files before `git add .`) |
| 6. Commit | `git commit -m "<type>(<scope>): <summary>"` |
| 7. Push | `git push origin <branch>` |

Commit types: `feat`, `fix`, `refactor`, `test`, `docs`, `chore`, `perf`

## Common Mistakes

- Skipping sanitizer check before commit
- Using `git add .` without reviewing untracked files
- Writing commit messages in past tense ("added" instead of "add")
- Forgetting scope in commit message (e.g. `feat: add timeout` instead of `feat(tcp): add timeout`)

## Workflow Routing

| Intent | Reference |
|--------|-----------|
| Commit and push code changes | [commit.md](references/commit.md) |
