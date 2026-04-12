---
name: c-project-init
description: >
  Use when creating a new C project from scratch, or when asked to scaffold,
  init, or bootstrap a C library or application project with CMake.
---

# C Project Init

## When to Use

- User asks to create, scaffold, init, or bootstrap a C project

**When NOT to Use:** building existing projects, writing code in existing projects, committing

## STOP — Collect ALL Inputs First

Ask the user for ALL of these before generating anything:

1. Project name
2. Project type (library or application)
3. Platform (cross-platform, windows, or unix)
4. Description
5. Author
6. Email

**Do NOT assume defaults. Do NOT start generating before asking.**

## STOP — Required Before Generating ANY Files

**MANDATORY:** Read `references/setup.md` **in full** before writing a single file.

1. **Project level:** Use `fileSearch` to search for `c-project-init/references/setup.md` within the current project.
2. **User level:** Search for `c-project-init/references/setup.md` under the user home directory (`~` on Unix, `%USERPROFILE%` on Windows).

**If not found at either level, STOP. Tell the user the reference is missing. Do NOT proceed.**

## Red Flags

- User gave partial inputs but not all 6 → STILL ASK for the rest
- About to generate files without having read setup.md this session
- setup.md read failed → STOP, do not generate
