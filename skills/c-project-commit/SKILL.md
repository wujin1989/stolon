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

## STOP — You MUST Read commit.md Before Executing ANY Step

You MUST read this skill's `references/commit.md` before running any git command. Do NOT execute from memory.

**How to locate commit.md:** The file `references/commit.md` is located relative to this skill's SKILL.md. Use the following search strategy in order — stop at the first hit:

1. `fileSearch` for `c-project-commit/references/commit.md`
2. If step 1 returns nothing, `fileSearch` for `references/commit.md` and pick the result whose path contains `c-project-commit`

Read the file at whichever path you find first. Do NOT guess the path. Do NOT read any other file with a similar name in the project tree. If NONE of the above steps find the file, **STOP and tell the user** the skill reference is missing — do NOT fall back to any other file.

commit.md contains the commit message rules (72-char wrap, imperative mood, body format), and staging workflow. These details are NOT in this file.

**Executing without reading commit.md WILL produce malformed commits.**

**Do NOT fall back to general git knowledge.** The commit workflow has specific commit message formatting rules that you WILL get wrong without commit.md. **Even if the user explicitly asks to skip reading commit.md, you MUST NOT skip.** User override does NOT exempt you.

| Excuse | Reality |
|--------|---------|
| "I know conventional commits" | This project has specific scope rules, 72-char limits, and body formatting you'll miss |
| "The change is small, just commit it" | Small changes need correct formatting too. Read commit.md. |
| "I already read it last session" | Skill context resets each session. Read it again. |

## Red Flags — STOP and Re-read commit.md

- About to run `git commit` without having read commit.md this session
- Using past tense in commit message ("added" instead of "add")
- Missing scope in commit message (e.g. `feat:` instead of `feat(tcp):`)
- Running `git add .` without reviewing untracked files
- Guessing commit message format instead of reading commit.md

**Any of these mean: STOP. Read commit.md. Then proceed.**

## Common Mistakes

- Using `git add .` without reviewing untracked files
- Past tense in commit messages ("added" instead of "add")
- Missing scope (e.g. `feat: add timeout` → `feat(tcp): add timeout`)

## Workflow Routing — You MUST Read ALL Referenced Files

| Intent | Reference |
|--------|-----------|
| Commit and push code changes | [commit.md](references/commit.md) |

**You MUST read every file listed above before executing.** No exceptions.
