# Project Setup

## Inputs

Ask the user before starting:

| Input | Example |
|-------|---------|
| Project name | `myproject` |
| Project type | `library` or `application` |
| Description | One-line project summary |
| Author | Name for LICENSE |
| Email | Email for LICENSE |
| Year | Year or range for LICENSE (e.g. `2026-2036`) |

## Steps

### 1. Copy Templates

Copy `templates/common/` into the project root, then copy the matching type (`templates/library/` or `templates/application/`) on top.

### 2. Replace Placeholders

| Placeholder | Value | Derivation |
|-------------|-------|------------|
| `{project}` | project name, lowercase with hyphens | As-is from user input (e.g. `hello-lib`) |
| `{PROJECT}` | uppercase, hyphens → underscores | e.g. `HELLO_LIB` |
| `{YEAR}` | current year | e.g. `2026` |
| `{AUTHOR}` | author name | From user input |
| `{EMAIL}` | email address | From user input |
| `{DESCRIPTION}` | one-line description | From user input |

Replace in all files. No `{...}` placeholder should remain after this step.

### 3. Rename Files

| From | To |
|------|----|
| `cmake/utils.cmake` | `cmake/{project}-utils.cmake` |
| `include/project.h` (library only) | `include/{project}.h` |

### 4. Verify

Search all files for the case-insensitive regex `\{(project|year|author|email|description)\}` to confirm no unreplaced placeholders remain.
