# Stolon

Reusable AI skills and agents that extend from one domain to another — like stolons in nature.

## Structure

```
stolon/
├── skills/          # Reusable skill packages
├── agents/          # Specialized AI agent configurations
├── mcps/            # Shared MCP servers
└── LICENSE          # MIT License
```

## Skills

| Name | Description |
|------|-------------|
| [c-development](skills/c-development/) | Cross-platform C project: init, code style, build system, debugging |

## Agents

| Name | Description |
|------|-------------|
| [code-expert](agents/code-expert/) | General-purpose coding expert, language-agnostic |
| [self-auditor](agents/self-auditor/) | Audits stolon files for consistency and rule compliance |

## Usage

Copy the relevant skills, agents, or MCP configs into your IDE's project structure. Adapt file paths to match your IDE's conventions.

### Sync Rules

Stolon is the **source of truth** for all shared steering content. Changes flow in one direction:

```
stolon  →  target projects
```

- Modify stolon first, then sync to projects.

## License

```
MIT License

Copyright (c) 2026-2036, Jin.Wu <wujin.developer@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
```
