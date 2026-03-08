# C Development

Cross-platform C project skill covering initialization, code style, build system, and debugging.
Supports Windows (MSVC), Linux (GCC/Clang), and macOS (Clang).

## Contents

- `steering/` — AI steering files
- `templates/` — Project templates (common + library/application variants)

## Steering Files

| File | Inclusion | Description |
|------|-----------|-------------|
| codestyle.md | fileMatch `*.{c,h}` | Code style, naming, license header, project structure |
| build.md | fileMatch `CMakeLists.txt,*.cmake` | Build system configuration |
| init.md | manual | Post-init checklist: user-input placeholders and verification |

When syncing to an IDE that uses front-matter (e.g. Kiro), convert the Inclusion column to the IDE's native mechanism.

## Templates

Templates are split into `common/` (shared by all project types), `library/` (library projects), and `application/` (executable projects).

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

### library/ (library projects)

| File | Description |
|------|-------------|
| CMakeLists.txt | Root CMake with add_library, sanitizers, coverage, install headers |
| include/project.h | Umbrella header template (rename to `{project}.h`) |
| examples/CMakeLists.txt | Examples CMake (empty scaffold) |

### application/ (executable projects)

| File | Description |
|------|-------------|
| CMakeLists.txt | Root CMake with add_executable, sanitizers, coverage, install to bin |

## Usage

1. Copy all files from `common/` into your project root
2. Copy files from either `library/` or `application/` depending on your project type
3. Replace placeholders and rename files (see below)

## Placeholders

Replace before use:
- `{project}` — project folder name in lowercase, including hyphens if any (e.g. `mylib`, `hello-lib`). Must match the directory name.
- `{PROJECT}` — project folder name in UPPERCASE, hyphens become underscores (e.g. `MYLIB`, `HELLO_LIB`)
- `{YEAR}`, `{AUTHOR}`, `{EMAIL}` — license info
- `{DESCRIPTION}` — one-line project description (ask user)

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

## MCP Servers

| Platform | Debugger | MCP | Status |
|----------|----------|-----|--------|
| Windows | CDB/WinDbg | svnscha/mcp-windbg | Available (`pip install mcp-windbg`, requires Python >= 3.10) |
| macOS/Linux | LLDB | stass/lldb-mcp | Available (`git clone`) |
| Linux | GDB | signal-slot/mcp-gdb | Available (`uvx mcp-gdb`) |

### MCP Remote Debugging

Start a debug server, then tell the AI agent to connect. The agent handles breakpoints, stepping, and variable inspection via natural language.

```bash
# Windows (CDB) — pick any free port
cdb -server tcp:port=5005 -o out\Debug\<program>.exe
# Then tell the agent: tcp:Port=5005,Server=localhost

# Linux (GDB)
gdb -ex "target remote :5005" ./out/<program>

# macOS (LLDB)
lldb ./out/<program>
```

## Dependencies

Install these before using this skill:

| Tool | Purpose | Install |
|------|---------|---------|
| CMake >= 3.16 | Build system | [cmake.org](https://cmake.org/download/) |
| C compiler | MSVC / GCC / Clang | VS2022 (Windows), `apt install build-essential` (Linux), Xcode CLT (macOS) |
| clang-format | Code formatting | Included with LLVM: [llvm.org](https://releases.llvm.org/) |
| lcov + genhtml | Coverage (Linux/macOS) | `apt install lcov` / `brew install lcov` |
| OpenCppCoverage | Coverage (Windows) | [opencppcoverage.com](https://github.com/OpenCppCoverage/OpenCppCoverage) |
| GDB | Debugger (Linux) | `apt install gdb` |
| LLDB | Debugger (macOS) | Included with Xcode CLT |
| WinDbg | Debugger (Windows) | Microsoft Store or `winget install Microsoft.WinDbg` |
