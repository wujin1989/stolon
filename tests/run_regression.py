#!/usr/bin/env python3
"""Regression baseline diff tool for stolon.

Compares current baselines against a git reference (default: HEAD) to detect
unintended changes after skill modifications.

Usage:
    python run_regression.py                    # diff all skills against HEAD
    python run_regression.py c-project-build    # diff one skill
    python run_regression.py --ref HEAD~3       # diff against 3 commits ago
    python run_regression.py --ref main         # diff against main branch
"""

import subprocess
import sys
from pathlib import Path

TESTS_DIR = Path(__file__).parent
EVALS_DIR = TESTS_DIR / "evals"


def git_diff_baselines(skill_name: str, ref: str) -> tuple[int, int, list[str]]:
    """Diff baselines for a skill against a git ref.

    Returns (changed_count, new_count, details).
    """
    baseline_path = f"tests/evals/{skill_name}/baselines"
    changed = 0
    new = 0
    details = []

    # Get list of changed files in baselines
    result = subprocess.run(
        ["git", "diff", "--name-status", ref, "--", baseline_path],
        capture_output=True, text=True, cwd=TESTS_DIR.parent,
        encoding="utf-8", errors="replace",
    )
    if result.returncode != 0:
        details.append(f"  git diff failed: {result.stderr.strip()}")
        return 0, 0, details

    for line in result.stdout.strip().splitlines():
        if not line:
            continue
        parts = line.split("\t", 1)
        status = parts[0]
        filepath = parts[1] if len(parts) > 1 else ""

        if status == "M":
            changed += 1
            details.append(f"  MODIFIED: {filepath}")
        elif status == "A":
            new += 1
            details.append(f"  NEW:      {filepath}")
        elif status == "D":
            details.append(f"  DELETED:  {filepath}")
        else:
            details.append(f"  {status}:  {filepath}")

    # Also check for untracked files
    result = subprocess.run(
        ["git", "ls-files", "--others", "--exclude-standard", "--", baseline_path],
        capture_output=True, text=True, cwd=TESTS_DIR.parent,
        encoding="utf-8", errors="replace",
    )
    for line in result.stdout.strip().splitlines():
        if line:
            new += 1
            details.append(f"  UNTRACKED: {line}")

    return changed, new, details


def show_diff_content(skill_name: str, ref: str):
    """Show actual diff content for changed baselines."""
    baseline_path = f"tests/evals/{skill_name}/baselines"
    result = subprocess.run(
        ["git", "diff", "--stat", ref, "--", baseline_path],
        capture_output=True, text=True, cwd=TESTS_DIR.parent,
        encoding="utf-8", errors="replace",
    )
    if result.stdout.strip():
        print(result.stdout)


def main():
    ref = "HEAD"
    skill_filter = None

    args = sys.argv[1:]
    i = 0
    while i < len(args):
        if args[i] == "--ref" and i + 1 < len(args):
            ref = args[i + 1]
            i += 2
        else:
            skill_filter = args[i]
            i += 1

    if skill_filter:
        skills = [skill_filter]
    else:
        skills = sorted(
            d.name for d in EVALS_DIR.iterdir()
            if d.is_dir() and (d / "baselines").is_dir()
        )

    total_changed = 0
    total_new = 0
    has_regression = False

    for skill in skills:
        print(f"\n Regression: {skill} ")
        changed, new, details = git_diff_baselines(skill, ref)
        total_changed += changed
        total_new += new

        if not details:
            print("  No changes (clean)")
        else:
            for d in details:
                print(d)
            if changed > 0:
                has_regression = True
                print(f"\n   {changed} baseline(s) changed  review needed")
                show_diff_content(skill, ref)

    print(f"\n===============================")
    print(f"  CHANGED:   {total_changed}")
    print(f"  NEW:       {total_new}")
    print(f"  Reference: {ref}")
    print(f"===============================")

    if has_regression:
        print("\n Baseline regressions detected. Review changes with:")
        print(f"  git diff {ref} -- tests/evals/*/baselines/")
        sys.exit(1)
    else:
        print("\n No regressions.")
        sys.exit(0)


if __name__ == "__main__":
    main()
