# Stolon

Reusable AI skills and agents that extend from one domain to another — like stolons in nature.

## Structure

```
stolon/
├── skills/          # Reusable skill packages
├── agents/          # Specialized AI agent configurations
└── mcps/            # Shared MCP servers
```

## Skills

| Name | Description |
|------|-------------|
| [c-project-init](skills/c-project-init/) | Initialize cross-platform C project with CMake, testing, sanitizers, and coverage |
| [c-project-debug](skills/c-project-debug/) | Cross-platform C project debugging (GDB/LLDB/MSVC) |

## Agents

| Name | Description |
|------|-------------|
| [code-expert](agents/code-expert/) | General-purpose coding expert, language-agnostic |

## Usage

### If your IDE supports skills

Copy skills directly into your IDE's skills directory:

```bash
cp -r skills/c-project-init/ <your-project-skills-dir>/
cp -r skills/c-project-debug/ <your-project-skills-dir>/
```

### If your IDE does not support skills

Copy steering files into your IDE's supported location (e.g. a steering or rules directory), and scripts into the project root:

```bash
cp skills/c-project-init/steering/* <your-project-steering-dir>/
cp skills/c-project-debug/steering/* <your-project-steering-dir>/
cp -r skills/c-project-init/scripts/ <your-project>/scripts/
```

### MCP configurations

```bash
cp mcps/c-debugger.json <your-project-mcp-config>
```

### Placeholders

Replace before use: `{project}`, `{PROJECT}`, `{YEAR}`, `{AUTHOR}`, `{EMAIL}`

## License

[MIT](LICENSE)
