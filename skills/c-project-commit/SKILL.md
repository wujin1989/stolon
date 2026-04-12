---
name: c-project-commit
description: >
  Use when staging, committing, or pushing code changes in a C project.
---

# C Project Commit

## Overview

Stage, commit, and push code changes with conventional commit message formatting.

## When to Use

- User asks to commit, push, or check in code

**When NOT to Use:** building/testing only, writing code, fixing build errors

## STOP — You MUST Read commit.md Before Executing ANY Step

You MUST read [commit.md](references/commit.md) before running any git command. Do NOT execute from memory.

commit.md contains the commit message rules (72-char wrap, imperative mood, body format), and staging workflow. These details are NOT in this file.

**Executing without reading commit.md WILL produce malformed commits.**

### If commit.md Cannot Be Read — STOP COMPLETELY

If the file read fails (file not found, access denied, any error):

1. **Do NOT run any git command.**
2. Tell the user: "I cannot proceed — commit.md is required but could not be read. Please check the file exists at the expected path."
3. **Do NOT fall back to general git knowledge.** The commit workflow has specific commit message formatting rules that you WILL get wrong without commit.md.

## Common Mistakes

- Using `git add .` without reviewing untracked files
- Past tense in commit messages ("added" instead of "add")
- Missing scope (e.g. `feat: add timeout` → `feat(tcp): add timeout`)

## Workflow Routing

| Intent | Reference |
|--------|-----------|
| Commit and push code changes | [commit.md](references/commit.md) |
