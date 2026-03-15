# Skill Evals

Lightweight eval harness for testing agent skill instructions. Each skill has its own eval suite.

## Structure

```
tests/
├── run-eval.sh              # Universal runner
├── README.md
└── evals/
    ├── c-development/       # One directory per skill
    │   ├── prompts.json     # Prompt definitions + expected checks
    │   ├── checks.sh        # Deterministic check functions
    │   └── sample-outputs/  # Collected agent outputs
    │       ├── build_sanitizer/output.txt
    │       └── ...
    └── <next-skill>/
        ├── prompts.json
        ├── checks.sh
        └── sample-outputs/
```

## Usage

```bash
# Run all skills
bash tests/run-eval.sh

# Run one skill
bash tests/run-eval.sh c-development

# Run one skill with custom output directory
bash tests/run-eval.sh c-development path/to/outputs
```

## Adding a new skill eval

1. Create `tests/evals/<skill-name>/`
2. Add `prompts.json` — array of prompts with `id`, `skill_ref`, `expected_checks`
3. Add `checks.sh` — bash functions named `check_<name>`, return 0 for pass
4. Collect agent outputs into `sample-outputs/<prompt-id>/output.txt`
5. Run: `bash tests/run-eval.sh <skill-name>`

## Collecting outputs

Give each prompt to the agent with the skill loaded. Save results:

- Text responses → `<prompt-id>/output.txt`
- Generated projects → `<prompt-id>/project/` (directory with files)

## Check types

| Input | When to use | Example |
|-------|-------------|---------|
| `$project_dir` | Prompt generates files | `check_has_cmakelists` |
| `$output_text` | Prompt gets text advice | `check_mentions_asan_clean` |
