# Stolon

Reusable AI skills that extend from one domain to another — like stolons in nature.

## Structure

```
stolon/
├── skills/
│   ├── c-project-init/
│   │   ├── SKILL.md
│   │   └── references/
│   │       └── setup.md        # Project scaffolding instructions
│   ├── c-project-build/
│   │   ├── SKILL.md
│   │   └── references/
│   │       └── build.md        # Build/test/coverage workflow
│   ├── c-project-commit/
│   │   ├── SKILL.md
│   │   └── references/
│   │       └── commit.md       # Pre-commit validation + message format
│   └── c-project-style/
│       ├── SKILL.md
│       └── references/
│           └── style.md        # C11 cross-platform code style guide
├── tests/
│   ├── run_eval.py             # Universal eval runner
│   ├── README.md               # Eval harness documentation
│   └── evals/
│       ├── c-project-init/     # 5 prompts, 86 checks
│       ├── c-project-build/    # 7 prompts, 31 checks
│       ├── c-project-commit/   # 5 prompts, 29 checks
│       └── c-project-style/    # 11 prompts, 38 checks
├── .gitignore
└── LICENSE
```

## Skills

| Name | Description |
|------|-------------|
| [c-project-init](skills/c-project-init/) | Scaffold a new C project (library or application) |
| [c-project-build](skills/c-project-build/) | Build, test, sanitizer, and coverage workflow |
| [c-project-commit](skills/c-project-commit/) | Pre-commit validation and conventional commit format |
| [c-project-style](skills/c-project-style/) | Cross-platform C11 code style reference (naming, layout, memory, tests) |

## Usage

Copy `skills/` into your project's skill directory (e.g. `.kiro/skills/`). Skills are self-contained and independent of each other.

### Sync Rules

Stolon is the source of truth for all shared skill content. Changes flow in one direction:

```
stolon  →  target projects
```

Modify stolon first, then sync to projects.

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
