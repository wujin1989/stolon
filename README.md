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
| [c-development](skills/c-development/) | Cross-platform C project: init, code style, build system, debugging |

## Agents

| Name | Description |
|------|-------------|
| [code-expert](agents/code-expert/) | General-purpose coding expert, language-agnostic |
| [self-auditor](agents/self-auditor/) | Audits stolon itself against its own steering rules and principles |

> Agents are internal to stolon and are NOT synced to target projects. Only `skills/` content (steering, templates, scripts) is synced.

## Usage

### If your IDE supports skills

Copy skills directly into your IDE's skills directory:

```bash
cp -r skills/c-development/ <your-project-skills-dir>/
```

### If your IDE does not support skills

Copy steering files into your IDE's supported location (e.g. a steering or rules directory), and scripts into the project root:

```bash
cp skills/c-development/steering/* <your-project-steering-dir>/
cp -r skills/c-project-init/scripts/ <your-project>/scripts/
```

### MCP configurations

```bash
cp mcps/c-debugger.json <your-project-mcp-config>
```

### Sync Rules

Stolon is the **source of truth** for all shared steering content. Changes flow in one direction:

```
stolon  →  projects (Xylem, etc.)
```

- **Modify stolon first**, then sync to projects
- **Never modify stolon to match a project** — if a project has a better version, port the improvement to stolon first, then re-sync
- When syncing to a project, only adapt: front-matter (IDE-specific), `{project}` → actual name, remove inapplicable sections (e.g. Executable options for a library project)
- Project-specific additions (e.g. `Custom helpers: cmake/xylem-utils.cmake`) stay in the project only

### Placeholders

Replace before use:
- `{project}`, `{PROJECT}`, `{YEAR}`, `{AUTHOR}`, `{EMAIL}`, `{DESCRIPTION}` — see [c-development Placeholders](skills/c-development/README.md#placeholders)

### Token Optimization

Steering files include a "Recommended trigger" comment indicating when they should be loaded. When syncing to your IDE, convert these to the IDE's native conditional inclusion mechanism to avoid loading all steering files into every conversation.

| Recommended trigger | Meaning |
|---------------------|---------|
| `auto-include when editing *.c / *.h` | Only load when C source files are in context |
| `auto-include when editing CMakeLists.txt / *.cmake` | Only load when build files are in context |
| `always include` | Load in every conversation |
| `manual include` | Only load when explicitly requested |

For example, an IDE that supports file-match based inclusion should configure `codestyle.md` to trigger only on `*.c` / `*.h` files, and `tech.md` only on `CMakeLists.txt` / `*.cmake` files. This keeps token usage minimal while ensuring the right context is available at the right time.

## License

[MIT](LICENSE)
