# Commit Workflow

## Overview

This skill drives the commit/push cycle for C11 CMake projects. It enforces
pre-commit validation and consistent commit message formatting.

Placeholders:
- `{name}` -- lowercase project name from `CMakeLists.txt`
- `{NAME}` -- uppercase form

## Pre-Commit Checklist

Before committing, run these checks in order. Stop on first failure.

### 1. Build (Debug)

```bash
rm -rf out
cmake -B out -G Ninja -DCMAKE_BUILD_TYPE=Debug -D{NAME}_ENABLE_TESTING=ON
cmake --build out -j $(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)
```

### 2. Test

```bash
ctest --test-dir out --output-on-failure
```

### 3. Sanitizer (ASAN + UBSAN)

```bash
rm -rf out
cmake -B out -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -D{NAME}_ENABLE_TESTING=ON \
  -D{NAME}_ENABLE_ASAN=ON \
  -D{NAME}_ENABLE_UBSAN=ON
cmake --build out -j $(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)
ctest --test-dir out --output-on-failure
```

If any step fails, fix the issue before committing. Do not skip sanitizer checks.

### 4. Format Check (optional)

If the project has `.clang-format`, verify formatting:

```bash
find src include -name '*.c' -o -name '*.h' | xargs clang-format --dry-run --Werror
```

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
# 1. Build + test
rm -rf out
cmake -B out -G Ninja -DCMAKE_BUILD_TYPE=Debug -D{NAME}_ENABLE_TESTING=ON
cmake --build out -j $(nproc)
ctest --test-dir out --output-on-failure

# 2. Sanitizer check
rm -rf out
cmake -B out -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -D{NAME}_ENABLE_TESTING=ON -D{NAME}_ENABLE_ASAN=ON -D{NAME}_ENABLE_UBSAN=ON
cmake --build out -j $(nproc)
ctest --test-dir out --output-on-failure

# 3. Stage + commit + push
git add -u
git commit -m "feat(tcp): add write timeout support"
git push origin main
```
