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

## STOP — You MUST Read setup.md Before Generating ANY Files

After collecting all inputs, you MUST read this skill's `references/setup.md` in full before writing a single file. Do NOT generate from memory or general knowledge.

**How to locate setup.md:** The file `references/setup.md` is located relative to this SKILL.md. Since the skill loader already resolved this skill's directory, read `references/setup.md` directly using the same directory prefix as this SKILL.md file. Do NOT use fileSearch — the path is deterministic. Do NOT read any other file with a similar name in the project tree.

setup.md contains project-specific conventions that CANNOT be inferred from general C/CMake knowledge — non-standard license formatting, specific CMake options, exact file contents, and platform-conditional logic. Every file you generate must match setup.md verbatim.

**Generating without reading setup.md WILL produce wrong output.** Every time.

**You MUST read setup.md in FULL (all 400+ lines).** Skimming the first few sections is not enough — platform-conditional logic, license formatting, and test harness details are spread throughout the file. If you stop reading early, you WILL miss critical conventions.

**Do NOT fall back to general C/CMake knowledge.** The project has non-standard conventions (====  license fences, specific CMake version, custom file tree, exact comment formatting) that you WILL get wrong without setup.md.

| Excuse | Reality |
|--------|---------|
| "I'll use standard conventions for now" | Standard conventions are WRONG for this project |
| "The user is waiting, I should generate something" | Wrong output wastes MORE time than waiting |
| "I can fix it later after reading setup.md" | You'll miss dozens of subtle differences |
| "Most C projects look similar" | This one has ====  fences, _Pragma("once"), specific sanitizer functions |

## Red Flags — STOP

- User gave partial inputs but not all 6 → STILL ASK for the rest
- User said "just do it" or "hurry" → STILL ASK
- About to generate files without having read setup.md this session
- "I know what a C project looks like" → setup.md has 400+ lines of project-specific conventions you don't know
- "I'll just use a standard CMake template" → the template has specific options, versions, and patterns
- setup.md read failed → STOP. Do not generate. Tell the user.

**Any of these mean: STOP. Collect all inputs. Read setup.md. Then generate.**

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
- Not verifying placeholders after generation — see setup.md for the verification regex

## Workflow Routing — You MUST Read ALL Referenced Files

| Intent | Reference |
|--------|-----------|
| Create a new C project | [setup.md](references/setup.md) |

**You MUST read every file listed above before executing.** No exceptions.
