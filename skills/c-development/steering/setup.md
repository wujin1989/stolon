# Project Setup

## Inputs

Ask the user before starting:

| Input | Example | Used in |
|-------|---------|---------|
| Project name | `hello-lib` | Directory name, file names, CMake targets |
| Project type | `library` or `application` | Template selection |
| Description | One-line summary | README.md |
| Author | Name | LICENSE, AUTHORS |
| Email | Address | LICENSE, AUTHORS |

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

- No `{...}` remains in any file
- `cmake --preset default` or `cmake -B out` succeeds
- README.md has a real description
- LICENSE and AUTHORS have correct year, name, and email
