#!/usr/bin/env python3
"""Regression baseline diff tool for stolon skills.

Compares current baselines against a git reference (default: HEAD) to detect
unintended changes after modifying skill instructions.

Usage:
    python run_diff.py                    # diff all skills against HEAD
    python run_diff.py c-project-build    # diff one skill against HEAD
    python run_diff.py --ref HEAD~3       # diff against 3 commits ago
    python run_diff.py --ref main         # diff against main branch
"""

import argparse
import subprocess
import sys
from pathlib import Path

TESTS_DIR = Path(__file__).parent
EVALS_DIR = TESTS_DIR / "evals"
REPO_ROOT = TESTS_DIR.parent


def git_show(ref: str, relpath: str) -> str | None:
    """Get file content at a git ref. Returns None if file doesn't exist."""
    try:
        result = subprocess.run(
            ["git", "show", f"{ref}:{relpath}"],
            capture_output=True, text=True, cwd=REPO_ROOT,
            encoding="utf-8", errors="replace",
        )
        if result.returncode == 0:
            return result.stdout
        return None
    except Exception:
        return None


def diff_text(old: str, new: str, label: str) -> list[str]:
    """Simple line-by-line diff, returns list of change descriptions."""
    old_lines = old.strip().splitlines()
    new_lines = new.strip().splitlines()
    changes = []

    if old_lines == new_lines:
        return changes

    # Find added lines
    old_set = set(old_lines)
    new_set = set(new_lines)

    added = new_set - old_set
    removed = old_set - new_set

    for line in removed:
        trimmed = line.strip()[:80]
        if trimmed:
            changes.append(f"  - REMOVED: {trimmed}")

    for line in added:
        trimmed = line.strip()[:80]
        if trimmed:
            changes.append(f"  + ADDED:   {trimmed}")

    return changes


def diff_skill(skill_name: str, ref: str) -> tuple[int, int, list[str]]:
    """Diff all baselines for a skill. Returns (unchanged, changed, details)."""
    skill_dir = EVALS_DIR / skill_name / "baselines"
    if not skill_dir.is_dir():
        return 0, 0, [f"  No baselines directory for {skill_name}"]

    unchanged = 0
    changed = 0
    details = []

    for baseline_dir in sorted(skill_dir.iterdir()):
        if not baseline_dir.is_dir():
            continue

        baseline_name = baseline_dir.name

        # Check output.txt
        output_file = baseline_dir / "output.txt"
        if output_file.exists():
            relpath = output_file.relative_to(REPO_ROOT).as_posix()
            old_content = git_show(ref, relpath)
            new_content = output_file.read_text(encoding="utf-8")

            if old_content is None:
                details.append(f"  {baseline_name}/output.txt: NEW FILE")
                changed += 1
            else:
                changes = diff_text(old_content, new_content, baseline_name)
                if changes:
                    details.append(f"  {baseline_name}/output.txt: CHANGED")
                    details.extend(changes)
                    changed += 1
                else:
                    unchanged += 1

        # Check project directory
        project_dir = baseline_dir / "project"
        if project_dir.is_dir():
            for f in sorted(project_dir.rglob("*")):
                if not f.is_file():
                    continue
                relpath = f.relative_to(REPO_ROOT).as_posix()
                old_content = git_show(ref, relpath)
                new_content = f.read_text(encoding="utf-8", errors="ignore")

                if old_content is None:
                    details.append(f"  {baseline_name}/{f.relative_to(baseline_dir)}: NEW FILE")
                    changed += 1
                else:
                    changes = diff_text(old_content, new_content, str(f.name))
                    if changes:
                        details.append(
                            f"  {baseline_name}/{f.relative_to(baseline_dir)}: CHANGED"
                        )
                        details.extend(changes[:5])  # limit detail lines
                        if len(changes) > 5:
                            details.append(f"    ... and {len(changes) - 5} more changes")
                        changed += 1
                    else:
                        unchanged += 1

    return unchanged, changed, details


def main():
    parser = argparse.ArgumentParser(description="Regression baseline diff")
    parser.add_argument("skill", nargs="?", help="Skill name to diff (default: all)")
    parser.add_argument("--ref", default="HEAD", help="Git ref to compare against (default: HEAD)")
    args = parser.parse_args()

    if args.skill:
        skills = [args.skill]
    else:
        skills = sorted(
            d.name for d in EVALS_DIR.iterdir()
            if d.is_dir() and (d / "baselines").is_dir()
        )

    total_unchanged = 0
    total_changed = 0
    has_changes = False

    for skill in skills:
        print(f"\n=== {skill} ===")
        unchanged, changed, details = diff_skill(skill, args.ref)
        total_unchanged += unchanged
        total_changed += changed

        if changed > 0:
            has_changes = True
            for line in details:
                print(line)
        else:
            print(f"  No changes ({unchanged} baselines unchanged)")

    print(f"\n===============================")
    print(f"  UNCHANGED: {total_unchanged}")
    print(f"  CHANGED:   {total_changed}")
    print(f"===============================")

    if has_changes:
        print("\nBaseline changes detected. Review above and either:")
        print("  1. Update baselines if changes are intentional")
        print("  2. Fix skill instructions if changes are unintended")
        sys.exit(1)
    else:
        print("\nNo baseline regressions detected.")
        sys.exit(0)


if __name__ == "__main__":
    main()
