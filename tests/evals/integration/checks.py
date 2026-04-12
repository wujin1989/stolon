"""Cross-skill integration checks.

Verifies that output from one skill is compatible with the next skill
in the chain: init -> style -> build -> debug -> commit.

Each check validates a handoff point between two skills.
"""

import re
from pathlib import Path


def directory_check(fn):
    fn.input_type = "directory"
    return fn


def text_check(fn):
    fn.input_type = "text"
    return fn


# --- init -> build: scaffolded project is buildable ---


@directory_check
def check_init_has_cmake_for_build(d: Path) -> bool:
    """init output must have CMakeLists.txt that build skill can configure."""
    cm = d / "CMakeLists.txt"
    if not cm.exists():
        return False
    text = cm.read_text(encoding="utf-8")
    return "project(" in text and "ENABLE_TESTING" in text


@directory_check
def check_init_has_ninja_compatible_cmake(d: Path) -> bool:
    """init output must use cmake_minimum_required >= 3.25 for Ninja Multi-Config."""
    cm = d / "CMakeLists.txt"
    if not cm.exists():
        return False
    text = cm.read_text(encoding="utf-8")
    return "cmake_minimum_required(VERSION 3.25)" in text


@directory_check
def check_init_has_test_target(d: Path) -> bool:
    """init output must have tests/ with CMakeLists.txt for ctest."""
    tc = d / "tests" / "CMakeLists.txt"
    return tc.exists()


@directory_check
def check_init_has_sanitizer_options(d: Path) -> bool:
    """init output must have ASAN/TSAN/UBSAN options for debug skill."""
    cm = d / "CMakeLists.txt"
    if not cm.exists():
        return False
    text = cm.read_text(encoding="utf-8")
    return (
        "ENABLE_ASAN" in text
        and "ENABLE_TSAN" in text
        and "ENABLE_UBSAN" in text
    )


@directory_check
def check_init_has_coverage_for_build(d: Path) -> bool:
    """init output must have coverage option for build skill coverage workflow."""
    cm = d / "CMakeLists.txt"
    if not cm.exists():
        return False
    text = cm.read_text(encoding="utf-8")
    return "ENABLE_COVERAGE" in text


# --- init -> style: scaffolded code follows style rules ---


@directory_check
def check_init_uses_pragma_once(d: Path) -> bool:
    """All .h files from init must use _Pragma("once"), not #pragma once."""
    for f in d.rglob("*.h"):
        if f.name == ".gitkeep":
            continue
        text = f.read_text(encoding="utf-8", errors="ignore")
        if "#pragma once" in text:
            return False
        if '_Pragma("once")' not in text and "_Pragma" not in text:
            return False
    return True


@directory_check
def check_init_uses_c_comments(d: Path) -> bool:
    """All .c/.h files from init must use /* */ comments, not //."""
    for f in d.rglob("*"):
        if f.suffix not in (".c", ".h") or f.name == ".gitkeep":
            continue
        text = f.read_text(encoding="utf-8", errors="ignore")
        for line in text.split("\n"):
            stripped = line.strip()
            if stripped.startswith("//"):
                return False
    return True


@directory_check
def check_init_has_license_headers(d: Path) -> bool:
    """All .c/.h files from init must start with license header (style rule)."""
    for f in d.rglob("*"):
        if f.suffix not in (".c", ".h") or f.name == ".gitkeep":
            continue
        text = f.read_text(encoding="utf-8", errors="ignore").lstrip()
        if not text.startswith("/**"):
            return False
    return True


@directory_check
def check_init_no_banned_functions(d: Path) -> bool:
    """init scaffolded code must not use sprintf/strcpy."""
    for f in d.rglob("*"):
        if f.suffix not in (".c", ".h") or f.name == ".gitkeep":
            continue
        text = f.read_text(encoding="utf-8", errors="ignore")
        if re.search(r"\bsprintf\s*\(", text):
            return False
        if re.search(r"\bstrcpy\s*\(", text):
            return False
    return True


# --- init -> commit: scaffolded project has git-committable structure ---


@directory_check
def check_init_has_gitignore(d: Path) -> bool:
    """init output must have .gitignore for clean commits."""
    gi = d / ".gitignore"
    if not gi.exists():
        return False
    text = gi.read_text(encoding="utf-8")
    return "out" in text or "build" in text


@directory_check
def check_init_gitignore_excludes_build(d: Path) -> bool:
    """init .gitignore must exclude build artifacts."""
    gi = d / ".gitignore"
    if not gi.exists():
        return False
    text = gi.read_text(encoding="utf-8")
    return "out/" in text or "out\n" in text or "/out" in text


# --- build -> debug: build output enables debug workflow ---


@text_check
def check_build_debug_enables_sanitizer_rebuild(text: str) -> bool:
    """Build skill Debug output must be compatible with debug skill sanitizer step."""
    return (
        bool(re.search(r"ENABLE_TESTING[=\s]+ON", text, re.I))
        and bool(re.search(r"cmake\s+--build", text))
    )


@text_check
def check_build_uses_debug_for_sanitizers(text: str) -> bool:
    """Build skill must use Debug type when ASAN is enabled (debug skill requires it)."""
    has_asan = bool(re.search(r"ENABLE_ASAN[=\s]+ON", text, re.I))
    if not has_asan:
        return True
    return bool(re.search(r"CMAKE_BUILD_TYPE[=\s]+Debug", text, re.I))


# --- build -> commit: build validates before commit ---


@text_check
def check_build_has_ctest_for_commit(text: str) -> bool:
    """Build skill output must include ctest (commit skill requires test step)."""
    return "ctest" in text and "--output-on-failure" in text


# --- debug -> commit: debug output leads to committable fix ---


@text_check
def check_debug_identifies_root_cause(text: str) -> bool:
    """Debug skill output should identify root cause (needed before commit)."""
    lower = text.lower()
    return (
        "root cause" in lower or "found" in lower or "identified" in lower
        or "the bug" in lower or "the issue" in lower or "the problem" in lower
        or "backtrace" in lower or "stack trace" in lower
        or "asan" in lower or "sanitizer" in lower
    )
