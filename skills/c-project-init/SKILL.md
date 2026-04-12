---
name: c-project-init
description: >
  Use when creating a new C project from scratch, or when asked to scaffold,
  init, or bootstrap a C library or application project with CMake.
---

# C Project Init

## Overview

Scaffold a new C project (library or application) with CMake, cross-platform support, test harness, and build scripts.

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

## STOP — Read setup.md Before Generating ANY Files

Read this skill's `references/setup.md` **in full** (all 400+ lines) before writing a single file. Do NOT generate from memory.

**How to locate:** Read `references/setup.md` relative to the directory containing this `SKILL.md`. Derive the path from where you loaded this file. Do NOT guess. Do NOT use `fileSearch`.

**If not found, STOP. Tell the user the reference is missing. Do NOT proceed.**

setup.md contains non-standard conventions (`====` license fences, `_Pragma("once")`, specific CMake version, exact file tree) that CANNOT be inferred from general C/CMake knowledge. Skimming is not enough — platform-conditional logic and formatting details are spread throughout.

## Red Flags — STOP

- User gave partial inputs but not all 6 → STILL ASK for the rest
- User said "just do it" or "hurry" → STILL ASK
- About to generate files without having read setup.md this session
- setup.md read failed → STOP. Do not generate.

## Common Mistakes

- Forgetting to ask for project type or platform
- Adding files not in the file tree (version.h, version.c, src/{name}.c)
- Using `include/{name}/{name}.h` instead of `include/{name}.h` (umbrella header is flat)
- Using SPDX headers instead of full MIT license block with `====` fences
- Using `#pragma once` instead of `_Pragma("once")`
- Creating a custom test framework instead of the specified tests/assert.h
- Using default year instead of current year for `{year}` placeholder

## Reference

| Intent | File |
|--------|------|
| Create a new C project | [setup.md](references/setup.md) |
