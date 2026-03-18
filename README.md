# Stolon

Reusable AI skills and agents that extend from one domain to another — like stolons in nature.

## Structure

```
stolon/
├── skills/
│   └── c-scaffold/
│       ├── SKILL.md            # Skill definition (metadata + routing table)
│       ├── README.md           # Human documentation
│       ├── references/         # Docs loaded into context as needed
│       └── assets/
│           └── templates/      # Project scaffolding
├── sub-agents/
│   ├── code-expert/
│   │   ├── system-prompt.md    # Agent behavior and instructions
│   │   └── README.md           # Human documentation
│   └── self-auditor/
│       ├── system-prompt.md    # Agent behavior and instructions
│       ├── principles.md       # Self-check rules
│       └── README.md           # Human documentation
├── tests/
│   ├── run_eval.py             # Universal eval runner
│   ├── README.md               # Eval harness documentation
│   └── evals/                  # One directory per skill
├── .gitignore
└── LICENSE
```

## Skills

| Name | Description |
|------|-------------|
| [c-scaffold](skills/c-scaffold/) | Scaffold cross-platform C projects from templates |

## Agents

| Name | Description |
|------|-------------|
| [code-expert](sub-agents/code-expert/) | Subagent: coding expert |
| [self-auditor](sub-agents/self-auditor/) | Subagent: rule compliance auditor |

## Usage

### Claude Code

Copy `skills/` and `sub-agents/` into your project's `.claude/` directory. Skills are auto-discovered via `SKILL.md`, agents available via subagent spawn.

### Kiro

Copy `skills/` into `.kiro/skills/`, `sub-agents/` into `.kiro/agents/`.

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
