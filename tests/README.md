# Skill Evals

Lightweight Python eval harness for testing agent skill instructions. Each skill has its own eval suite.

## How it works

1. `prompts.json` defines prompts that simulate user requests
2. `baselines/` contains AI-generated responses (baseline)
3. `checks.py` has deterministic functions that verify responses follow skill rules
4. `run_eval.py` feeds outputs into checks and reports pass/fail

## Structure

```
tests/
├── run_eval.py              # Universal runner
├── README.md
└── evals/
    ├── c-scaffold/       # One directory per skill
    │   ├── prompts.json     # Prompt definitions + expected checks
    │   ├── checks.py        # Deterministic check functions
    │   └── baselines/        # AI-generated baseline outputs
    └── <next-skill>/
        ├── prompts.json
        ├── checks.py
        └── baselines/
```

## Usage

```bash
# Run all skills
python tests/run_eval.py

# Run one skill
python tests/run_eval.py c-scaffold

# Run one skill with custom output directory
python tests/run_eval.py c-scaffold path/to/outputs
```

## Workflow after changing skill instructions

1. Modify skill references (e.g. `skills/c-scaffold/references/setup.md`)
2. Ask the AI to re-read the updated skill instructions and regenerate sample-outputs
3. Run `python tests/run_eval.py` to verify checks still pass
4. If checks fail, either fix the skill instructions or update checks.py
5. Commit the updated baselines as the new baseline

## Adding a new skill eval

1. Create `tests/evals/<skill-name>/`
2. Add `prompts.json` — array of prompts with `id`, `skill_ref`, `expected_checks`
3. Add `checks.py` — functions named `check_<name>` decorated with `@directory_check` or `@text_check`
4. Ask the AI to read the skill instructions and generate responses for each prompt
5. Save AI responses into `baselines/<prompt-id>/output.txt` (text) or `baselines/<prompt-id>/project/` (directory)
6. Run: `python tests/run_eval.py <skill-name>`
7. Commit baselines

## Writing checks

```python
from pathlib import Path

def directory_check(fn):
    fn.input_type = "directory"
    return fn

def text_check(fn):
    fn.input_type = "text"
    return fn

@directory_check
def check_has_cmakelists(d: Path) -> bool:
    return (d / "CMakeLists.txt").exists()

@text_check
def check_mentions_asan(text: str) -> bool:
    return "ASAN" in text.upper()
```

- `@directory_check`: receives a `Path` to the generated project directory
- `@text_check`: receives the agent's text response as a string
- Return `True` for pass, `False` for fail
