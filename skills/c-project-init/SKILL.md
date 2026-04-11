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

### If setup.md Cannot Be Read — STOP COMPLETELY

If the file read fails (file not found, access denied, any error):

1. **Do NOT generate any files.** Not even "standard" ones.
2. Tell the user: "I cannot proceed — setup.md is required but could not be read. Please check the file exists at the expected path."
3. **Do NOT fall back to general C/CMake knowledge.** The project has non-standard conventions (====  license fences, specific CMake version, custom file tree, exact comment formatting) that you WILL get wrong without setup.md.

| Excuse | Reality |
|--------|---------|
| "I'll use standard conventions for now" | Standard conventions are WRONG for this project |
| "The user is waiting, I should generate something" | Wrong output wastes MORE time than waiting |
| "I can fix it later after reading setup.md" | You'll miss dozens of subtle differences |
| "Most C projects look similar" | This one has ====  fences, _Pragma("once"), specific sanitizer functions |

### Red Flags — STOP If You're About to Skip Reading

- "I know what a C project looks like" → setup.md has project-specific conventions you don't know
- "I'll just use a standard CMake template" → the template has specific options, versions, and patterns
- "setup.md is probably just confirming what I know" → it contains 400+ lines of precise specifications
- setup.md read failed → STOP. Do not generate. Tell the user.

## Common Mistakes

- Forgetting to ask for project type or platform
- Not reading setup.md before generating files
- **Generating files when setup.md read fails** — this is the #1 failure mode
- Adding files not in the file tree (version.h, version.c, src/{name}.c, etc.)
- Using `include/{name}/{name}.h` instead of `include/{name}.h` (umbrella header is flat)
- Using SPDX headers instead of the full MIT license header block with `====` fences
- Using wrong CMake version (setup.md specifies exactly 3.25)
- Using `#pragma once` instead of `_Pragma("once")`
- Creating a custom test framework instead of using the specified tests/assert.h
- Skipping docs/, examples/, or cmake/ directories
- Using `#ifndef` include guards instead of `_Pragma("once")`
- Writing a "standard" README instead of the exact template from setup.md
- Using default year instead of current year for `{year}` placeholder

## Post-Generation Verification

After generating all files, run this regex across every generated file:

```
(?<!\$)\{(name|NAME|year|author|email|description|LICENSE_HEADER)\}
```

No matches should remain (the negative lookbehind `(?<!\$)` excludes cmake `${NAME}` expansions). Any remaining placeholder means you missed a substitution.

## Workflow Routing

| Intent | Reference |
|--------|-----------|
| Create a new C project | [setup.md](references/setup.md) |
