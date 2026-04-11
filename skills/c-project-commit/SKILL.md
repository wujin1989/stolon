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

**Even if the user explicitly asks to skip validation, you MUST run the full checklist.** No exceptions.

## STOP — You MUST Read commit.md Before Executing ANY Step

You MUST read [commit.md](references/commit.md) before running any build, test, or git command. Do NOT execute from memory.

commit.md contains the exact cmake flags, sanitizer options, commit message rules (72-char wrap, imperative mood, body format), and staging workflow. These details are NOT in this file.

**Executing without reading commit.md WILL produce incorrect builds or malformed commits.**

### If commit.md Cannot Be Read — STOP COMPLETELY

If the file read fails (file not found, access denied, any error):

1. **Do NOT run any build, test, or git command.**
2. Tell the user: "I cannot proceed — commit.md is required but could not be read. Please check the file exists at the expected path."
3. **Do NOT fall back to general git/CMake knowledge.** The commit workflow has specific cmake flags, sanitizer options, and commit message formatting rules that you WILL get wrong without commit.md.

| Excuse | Reality |
|--------|---------|
| "I know conventional commits" | This project has specific scope rules, 72-char limits, and body formatting you'll miss |
| "I'll just build and commit" | The pre-commit checklist requires TWO separate builds (normal + sanitizer) — skipping either pushes broken code |
| "The change is small, just commit it" | Small changes cause segfaults. The sanitizer step is mandatory. |

### Red Flags — STOP If You're Thinking This

- "Only changed docs/config, no need to build" → Build system changes can break compilation
- "Tests passed, sanitizer is redundant" → ASAN catches UB that tests miss
- "Change is tiny, can't break anything" → Tiny changes cause segfaults
- "User asked to skip" → You MUST run pre-commit checks. No exceptions.
- "CI will catch it" → Fix locally. Don't push unverified code.

## Common Mistakes

- Skipping sanitizer check before commit
- Using `git add .` without reviewing untracked files
- Past tense in commit messages ("added" instead of "add")
- Missing scope (e.g. `feat: add timeout` → `feat(tcp): add timeout`)

## Workflow Routing

| Intent | Reference |
|--------|-----------|
| Commit and push code changes | [commit.md](references/commit.md) |
