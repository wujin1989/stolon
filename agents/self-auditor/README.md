# Self Auditor Agent

Audits all changes in the stolon repository against its own steering rules, templates, and documentation for consistency and correctness.

## Usage

Switch to this agent when making any changes to the repository. The agent will validate changes against the relevant rules.

## Sync Note

`.kiro/agents/self-auditor.md` is a merged, standalone copy of this agent's `system-prompt.md` and `principles.md`, formatted for IDE consumption. When updating files in this directory, also update `.kiro/agents/self-auditor.md` to keep them in sync.

## Covers

- Steering files: self-check against principles in `principles.md`
- Templates: verify placeholders are correct, code examples comply with `style.md`
- README files: verify documentation matches actual file structure and placeholders
- Cross-file consistency: changes in one file are reflected in all related files
