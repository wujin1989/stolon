---
name: c-development
description: >
  Use when creating, building, testing, debugging, or deploying cross-platform C projects.
  Covers project initialization from templates, code style and naming conventions,
  CMake build system configuration, sanitizer and coverage testing, debugger-assisted
  diagnosis, dependency resolution, and release packaging.
  Supports Windows (MSVC), Linux (GCC/Clang), and macOS (Apple Clang).
---

# C Development Skill

Cross-platform C project skill. Covers the full lifecycle: init → code → build → test → debug → deploy.

## Workflow Routing

Match the user's intent to a reference document below. Load only the document(s) needed.

| Intent | Reference | Key Topics |
|--------|-----------|------------|
| Create a new C project, scaffold from template | [setup.md](references/setup.md) | Template copy, placeholder replacement, file rename |
| Code style, naming, license header, add a module | [style.md](references/style.md) | Prefix rule, naming table, header format |
| Configure CMake, add sources/tests, build options | [build.md](references/build.md) | CMake options, build/test/sanitizer/coverage commands |
| Write or run tests, sanitizer testing, coverage | [test.md](references/test.md) | ASSERT macro, test structure, sanitizer matrix |
| Diagnose crash, leak, race, wrong output | [debug.md](references/debug.md) | Diagnosis order, common pitfalls, analysis procedures |
| Release, install, package, CI | [deploy.md](references/deploy.md) | Release checklist, versioning, CPack, CI matrix |
| Missing tool, library, or header at build time | [deps.md](references/deps.md) | Install commands, common errors, resolution rules |

## Quick Reference

- Language standard: C11
- Build system: CMake >= 3.16
- Output directory: `out/`
- Formatting: `.clang-format` (provided in templates)
- Test framework: custom `ASSERT()` macro in `tests/assert.h` — no external dependencies
- Platform targets: Windows (MSVC), Linux (GCC/Clang), macOS (Apple Clang)

## Project Templates

Templates live in `assets/templates/` within this skill directory:

```
assets/templates/
├── common/       # Shared: .clang-format, .gitignore, cmake/, tests/, platform/
├── library/      # Library: CMakeLists.txt, include/, examples/
└── application/  # Application: CMakeLists.txt
```

Setup copies `common/` + the type-specific directory into the project root, then replaces placeholders. See [setup.md](references/setup.md) for the full procedure.

## MCP Debugger Servers

This skill works with debugger MCP servers for interactive diagnosis:

| Platform | Debugger | MCP Server |
|----------|----------|------------|
| Windows | CDB/WinDbg | `svnscha/mcp-windbg` |
| macOS/Linux | LLDB | `stass/lldb-mcp` |
| Linux | GDB | `signal-slot/mcp-gdb` |

See [debug.md](references/debug.md) for diagnosis workflows and MCP connection instructions.
