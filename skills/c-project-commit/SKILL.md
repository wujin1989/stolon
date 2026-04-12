---
name: c-project-commit
description: >
  Use when staging, committing, or pushing code changes in a C project.
---

# C Project Commit

## Overview

Every commit requires reading commit.md first — no exceptions. Enforces conventional commit formatting with project-specific scope rules.

## When to Use

- User asks to commit, push, or check in code

**When NOT to Use:** building/testing only, writing code, fixing build errors

## STOP — Read commit.md Before ANY Git Command

Read this skill's `references/commit.md` before executing. Do NOT commit from memory.

**How to locate:** Read `references/commit.md` relative to the directory containing this `SKILL.md`. Derive the path from where you loaded this file. Do NOT guess. Do NOT use `fileSearch`.

**If not found, STOP. Tell the user the reference is missing. Do NOT proceed.**

commit.md contains commit message rules (72-char wrap, imperative mood, scope rules, body format) and staging workflow. General git knowledge will produce malformed commits.

## Red Flags — STOP and Re-read commit.md

- About to run `git commit` without having read commit.md this session
- Using past tense ("added" instead of "add")
- Missing scope (`feat:` instead of `feat(tcp):`)
- Running `git add .` without reviewing untracked files

## Common Mistakes

- Using `git add .` without reviewing untracked files
- Past tense in commit messages ("added" instead of "add")
- Missing scope (e.g. `feat: add timeout` → `feat(tcp): add timeout`)

## Reference

| Intent | File |
|--------|------|
| Commit and push code changes | [commit.md](references/commit.md) |
