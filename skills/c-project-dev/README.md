# C Project Dev

Code style and build system steering for cross-platform C projects.
Used during daily development — synced to the project's steering directory.

## Contents

- `steering/` — AI steering files (codestyle, build system)

## Steering Files

| File | Inclusion | Description |
|------|-----------|-------------|
| codestyle.md | fileMatch `*.{c,h}` | Code style, naming, license header, project structure |
| tech.md | fileMatch `CMakeLists.txt,*.cmake` | Build system configuration |

When syncing to an IDE that uses front-matter (e.g. Kiro), `fileMatch` inclusion files require a `fileMatchPattern` field. Use the file patterns from the Inclusion column above.
