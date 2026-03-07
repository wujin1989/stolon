# Self Auditor Agent

Audits all changes in the stolon repository against its own steering rules, templates, and documentation for consistency and correctness.

## Usage

Switch to this agent when making any changes to the repository. The agent will automatically validate changes against the relevant rules before committing.

## Covers

- Steering files: self-check against 8 principles in `principles.md`
- Templates: verify placeholders are correct, code examples comply with `codestyle.md`
- Scripts: verify they match the instructions in `tech.md`
- README files: verify documentation matches actual file structure and placeholders
- Cross-file consistency: changes in one file are reflected in all related files
