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

Copy skills into your project's skills directory:

```bash
cp -r skills/c-project-init/ <your-project-skills-dir>/
cp -r skills/c-project-debug/ <your-project-skills-dir>/
```

Copy MCP configurations into your project:

```bash
cp mcps/c-debugger.json <your-project-mcp-config>
```

Then replace placeholders (`{project}`, `{PROJECT}`, `{YEAR}`, `{AUTHOR}`, `{EMAIL}`) with your actual values.

## License

[MIT](LICENSE)
