# C Project Init

Initialize a cross-platform C project with CMake, testing, sanitizers, and coverage.
Supports Windows (MSVC), Linux (GCC/Clang), and macOS (Clang).

## Contents

- `steering/` — AI steering files (codestyle, build system, product template)
- `templates/` — Project templates (common + lib/app variants)
- `scripts/` — Dependency installation scripts (sh + ps1)

## Steering Files

| File | Inclusion | Description |
|------|-----------|-------------|
| codestyle.md | fileMatch `*.{c,h}` | Code style, naming, license header, project structure |
| tech.md | fileMatch `CMakeLists.txt,*.cmake` | Build system configuration |
| product.md | manual | Project description template (fill on init, then change to always-include) |
| debug.md | manual | Debugging guide with platform debuggers and MCP servers |

When syncing to an IDE that uses front-matter (e.g. Kiro), `manual` inclusion files require a `description` field. Use the Description column value above.

## Templates

Templates are split into `common/` (shared by all project types), `lib/` (library projects), and `app/` (executable projects).

### common/ (shared)

| File | Description |
|------|-------------|
| .clang-format | LLVM-based clang-format config |
| .gitignore | Standard C project gitignore |
| AUTHORS | Authors file template |
| LICENSE | MIT license template |
| README.md | Project README template |
| cmake/utils.cmake | Helper functions (apply_sanitizer, add_test) |
| docs/build.md | Build instructions (multi/single-config, coverage, sanitizers) |
| src/platform/platform.h | Platform abstraction umbrella header |
| src/platform/win/ | Windows platform code directory |
| src/platform/unix/ | Linux/macOS platform code directory |
| tests/CMakeLists.txt | Test CMake with coverage targets |
| tests/assert.h | Test assertion macro (ASSERT) |

### lib/ (library projects)

| File | Description |
|------|-------------|
| CMakeLists.txt | Root CMake with add_library, sanitizers, coverage, install headers |
| include/project.h | Umbrella header template (rename to `{project}.h`) |
| examples/CMakeLists.txt | Examples CMake (empty scaffold) |

### app/ (executable projects)

| File | Description |
|------|-------------|
| CMakeLists.txt | Root CMake with add_executable, sanitizers, coverage, install to bin |

## Usage

1. Copy all files from `common/` into your project root
2. Copy files from either `lib/` or `app/` depending on your project type
3. Replace placeholders and rename files (see below)

## Placeholders

Replace before use:
- `{project}` — project folder name in lowercase, including hyphens if any (e.g. `mylib`, `hello-lib`). Must match the directory name.
- `{PROJECT}` — project folder name in UPPERCASE, hyphens become underscores (e.g. `MYLIB`, `HELLO_LIB`)
- `{YEAR}`, `{AUTHOR}`, `{EMAIL}` — license info

### Derived Forms

The placeholder value is used as-is in file names and CMake identifiers. In C identifiers (function names, type names, macros), hyphens are replaced with underscores:

| Context | `{project}` = `hello-lib` | `{PROJECT}` = `HELLO_LIB` |
|---------|--------------------------|---------------------------|
| File names | `hello-lib-list.c` | — |
| CMake target | `add_library(hello-lib ...)` | — |
| CMake options | — | `HELLO_LIB_ENABLE_TESTING` |
| CMake functions | `hello-lib_apply_sanitizer(...)` | — |
| C function prefix | `hello_lib_list_insert()` | — |
| C type prefix | `hello_lib_list_t` | — |
| Include directory | `include/hello-lib/` | — |
| Umbrella header | `hello-lib.h` | — |

> When the project name has no hyphens (e.g. `xylem`), all forms are identical — no conversion needed.

Also rename:
- `cmake/utils.cmake` → `cmake/{project}-utils.cmake`
- (lib only) `include/project.h` → `include/{project}.h`, `include/` directory → `include/{project}/`

## Dependencies

- Linux/macOS: `scripts/install-deps.sh`
- Windows: `scripts/install-deps.ps1` (requires admin)
- Visual Studio 2022 with C++ workload must be installed separately
