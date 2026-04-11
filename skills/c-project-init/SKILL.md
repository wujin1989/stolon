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

## STOP — Required Before Generating Any Files

You MUST ask the user for ALL of these inputs first:

1. Project name
2. Project type (library or application)
3. Platform (cross-platform, windows, or unix)
4. Description
5. Author
6. Email

**Do NOT assume defaults or start generating files before asking.**

### Red Flags — STOP If You're About to Skip Asking

- User gave project name but not type → STILL ASK
- User gave most inputs but not platform → STILL ASK
- User said "just do it" or "hurry" → STILL ASK
- You think you can infer the type from context → STILL ASK

## STOP — You MUST Read setup.md Before Generating ANY Files

After collecting all inputs, you MUST read [setup.md](references/setup.md) in full before writing a single file. Do NOT generate from memory or general knowledge.

setup.md contains project-specific conventions that CANNOT be inferred from general C/CMake knowledge — non-standard license formatting, specific CMake options, exact file contents, and platform-conditional logic. Every file you generate must match setup.md verbatim.

**Generating without reading setup.md WILL produce wrong output.** Every time.

### Red Flags — STOP If You're About to Skip Reading

- "I know what a C project looks like" → setup.md has project-specific conventions you don't know
- "I'll just use a standard CMake template" → the template has specific options, versions, and patterns
- "setup.md is probably just confirming what I know" → it contains 400+ lines of precise specifications

## Common Mistakes

- Forgetting to ask for project type or platform
- Not reading setup.md before generating files
- Adding files not in the file tree (version.h, version.c, etc.)
- Using SPDX headers instead of the full MIT license header block
- Using wrong CMake version (setup.md specifies the exact version)
- Creating a custom test framework instead of using the specified tests/assert.h
- Skipping docs/, examples/, or cmake/ directories

## Workflow Routing

| Intent | Reference |
|--------|-----------|
| Create a new C project | [setup.md](references/setup.md) |
