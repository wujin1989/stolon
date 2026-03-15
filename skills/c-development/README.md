# C Development

Cross-platform C project skill covering initialization, code style, build system, and debugging.
Supports Windows (MSVC), Linux (GCC/Clang), and macOS (Clang).

## Contents

```
c-development/
├── SKILL.md              # Skill definition (metadata + routing table)
├── README.md             # This file (for humans)
├── references/           # Docs loaded into context as needed
│   ├── setup.md          # Project setup checklist
│   ├── style.md          # Code style, naming, license header, project structure
│   ├── build.md          # Build system configuration
│   ├── test.md           # Test conventions, sanitizer/coverage testing
│   ├── debug.md          # Debugging rules, diagnosis order, common pitfalls
│   ├── deploy.md         # Release checklist, versioning, packaging, CI
│   └── deps.md           # Dependency diagnosis and resolution
└── assets/
    └── templates/        # Project scaffolding
        ├── common/       # Shared files (.clang-format, .gitignore, cmake/, tests/, etc.)
        ├── library/      # Library-specific (CMakeLists.txt, include/, examples/)
        └── application/  # Application-specific (CMakeLists.txt)
```

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

To use debugger MCPs, add the corresponding config to `.claude/settings.json`. See the project root `settings.json` for example configurations.

## Cross-Platform Pitfalls

| Pitfall | Detail |
|---------|--------|
| `long` size | 4 bytes on Windows x64, 8 bytes on Linux/macOS x64 — use `int32_t`/`int64_t` |
| Stack size | Windows 1MB default, Linux 8MB default |
| Uninitialized memory | May be zeroed in debug but not release — use ASAN to catch |
| Signals | Windows uses SEH, Unix uses POSIX signals |
| Paths | `\` on Windows, `/` on Unix |

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