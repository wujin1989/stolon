# Commit Workflow

## Overview

This skill drives the commit/push cycle for C11 CMake projects. It enforces
consistent commit message formatting and careful staging.

## Staging

### Review Changes First

Always show the diff before staging:

```bash
git diff
git status
```

### Stage Files

```bash
git add <files>
```

Or stage all tracked changes:

```bash
git add -u
```

Do not blindly `git add .` -- review untracked files first with `git status`.

## Commit Message Convention

Format:

```
<type>(<scope>): <summary>

<optional body>
```

### Type

| Type | When |
|------|------|
| `feat` | New feature or public API addition |
| `fix` | Bug fix |
| `refactor` | Code restructuring, no behavior change |
| `test` | Adding or updating tests |
| `docs` | Documentation only |
| `chore` | Build system, CI, tooling changes |
| `perf` | Performance improvement |

### Scope

Module name from the project (e.g. `tcp`, `udp`, `tls`, `loop`, `platform`).
Omit scope for cross-cutting changes.

### Summary

- Imperative mood ("add", not "added" or "adds")
- Lowercase first letter
- No period at end
- Max 72 characters

### Examples

```
feat(tcp): add heartbeat timeout callback
fix(udp): handle EAGAIN in sendto path
test(tls): add ALPN negotiation test
refactor(loop): extract timer reset logic
chore: update CMakeLists.txt for new module
```

### Body (optional)

Wrap at 72 characters. Explain *why*, not *what* (the diff shows what).

## Commit

```bash
git commit -m "<type>(<scope>): <summary>"
```

For multi-line messages:

```bash
git commit
```

This opens the editor. Write the summary line, blank line, then body.

## Push

```bash
git push origin <branch>
```

If the remote branch does not exist yet:

```bash
git push -u origin <branch>
```

## Quick Reference

Full cycle for a typical change:

```bash
# 1. Review + stage
git diff
git status
git add -u

# 2. Commit + push
git commit -m "feat(tcp): add write timeout support"
git push origin main
```
