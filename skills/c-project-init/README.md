# C Project Init

Initialize a cross-platform C project with CMake, testing, sanitizers, and coverage.
Supports Windows (MSVC), Linux (GCC/Clang), and macOS (Clang).

## Contents

- `steering/` — AI steering files (codestyle, build system, license, product template)
- `templates/` — CMake project templates (root CMakeLists.txt, utils, tests)
- `scripts/` — Dependency installation scripts (sh + ps1)

## Steering Files

| File | Inclusion | Description |
|------|-----------|-------------|
| codestyle.md | fileMatch `*.{c,h}` | Code style, naming, project structure |
| tech.md | fileMatch `CMakeLists.txt,*.cmake` | Build system configuration |
| license.md | auto | License header requirement |
| product.md | auto | Project description template (fill on init) |

## Templates

| File | Description |
|------|-------------|
| CMakeLists.txt | Root CMake with sanitizers, coverage, testing |
| cmake/utils.cmake | Helper functions (apply_sanitizer, add_test) |
| tests/CMakeLists.txt | Test CMake with coverage targets (OpenCppCoverage + lcov) |
| tests/assert.h | Test assertion macro (ASSERT) |
| examples/CMakeLists.txt | Examples CMake (empty scaffold) |
| include/project.h | Umbrella header template (rename to `{project}.h`) |
| README.md | Project README template |
| LICENSE | MIT license template |
| AUTHORS | Authors file template |
| .gitignore | Standard C project gitignore |
| .clang-format | LLVM-based clang-format config |
| docs/build.md | Build instructions template |

## Placeholders

Replace before use:
- `{project}` — project name in lowercase (e.g. `mylib`)
- `{PROJECT}` — project name in UPPERCASE (e.g. `MYLIB`)
- `{YEAR}`, `{AUTHOR}`, `{EMAIL}` — license info

Also rename:
- `cmake/utils.cmake` → `cmake/{project}-utils.cmake`
- `include/project.h` → `include/{project}.h`

## Dependencies

- Linux/macOS: `scripts/install-deps.sh`
- Windows: `scripts/install-deps.ps1` (requires admin)
- Visual Studio 2022 with C++ workload must be installed separately
