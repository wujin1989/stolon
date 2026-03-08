> Recommended trigger: manual include. Use when initializing a new project from templates.

# Project Initialization

After copying templates and replacing mechanical placeholders (`{project}`, `{PROJECT}`, `{YEAR}`, `{AUTHOR}`, `{EMAIL}`), the following placeholders require user input. Ask the user for each value, then replace it in all files where it appears.

| Placeholder | Ask user | Used in |
|-------------|----------|---------|
| `{DESCRIPTION}` | One-line project description | README.md |

## Post-Init Checklist

1. All `{placeholder}` values have been replaced — no `{...}` remains in any file
2. File renames completed (`cmake/utils.cmake` → `cmake/{project}-utils.cmake`, etc.)
3. README.md has a real project description, not a placeholder
4. LICENSE and AUTHORS have correct year, name, and email
