# C Project Debug

Cross-platform C project debugging skill. Supports Windows (MSVC), Linux (GDB), and macOS (LLDB).

## Contents

- `steering/` — Debugging best practices and platform-specific tips
- `scripts/` — Debugger installation scripts

> MCP configurations for debuggers are in the top-level [`mcps/debugger.json`](../../mcps/debugger.json).

## Steering Files

| File | Inclusion | Description |
|------|-----------|-------------|
| debug.md | manual | Debugging guide, platform differences, common pitfalls |

## MCP Servers

| Platform | Debugger | MCP | Status |
|----------|----------|-----|--------|
| Windows | CDB/WinDbg | svnscha/mcp-windbg | Available (`pip install mcp-windbg`) |
| Linux | GDB | signal-slot/mcp-gdb | Available (`uvx mcp-gdb`) |
| Cross-platform | WinDbg+LLDB | tonyredondo/debugger-mcp-server | Requires .NET 10 SDK |

## Dependencies

- Linux: `scripts/install-deps.sh`
- macOS: `scripts/install-deps.sh`
- Windows: `scripts/install-deps.ps1`
