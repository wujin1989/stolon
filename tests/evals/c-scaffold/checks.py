"""Deterministic checks for c-scaffold skill eval.

Each check function:
  - Named check_<name> matching the id in prompts.json expected_checks
  - Returns True (pass) or False (fail)
  - Has an 'input_type' attribute: "directory" for project dir, "text" for output text

Use the @directory_check / @text_check decorators to set input_type.
"""

import re
from pathlib import Path


def directory_check(fn):
    fn.input_type = "directory"
    return fn


def text_check(fn):
    fn.input_type = "text"
    return fn


# --- Setup checks: generated project structure ---


@directory_check
def check_has_cmakelists(d: Path) -> bool:
    return (d / "CMakeLists.txt").exists()


@directory_check
def check_no_unreplaced_placeholders(d: Path) -> bool:
    pattern = re.compile(r"\{(project|PROJECT|YEAR|AUTHOR|EMAIL|DESCRIPTION)\}")
    for f in d.rglob("*"):
        if f.is_file() and f.suffix in (".c", ".h", ".cmake", ".txt", ".md"):
            if pattern.search(f.read_text(encoding="utf-8", errors="ignore")):
                return False
    return True


@directory_check
def check_cmake_has_project_options(d: Path) -> bool:
    cm = d / "CMakeLists.txt"
    if not cm.exists():
        return False
    text = cm.read_text(encoding="utf-8")
    return all(
        opt in text
        for opt in ("ENABLE_TESTING", "ENABLE_ASAN", "ENABLE_TSAN", "ENABLE_UBSAN")
    )


@directory_check
def check_cmake_c_standard_11(d: Path) -> bool:
    cm = d / "CMakeLists.txt"
    return cm.exists() and "CMAKE_C_STANDARD 11" in cm.read_text(encoding="utf-8")


@directory_check
def check_has_clang_format(d: Path) -> bool:
    return (d / ".clang-format").exists()


@directory_check
def check_has_gitignore(d: Path) -> bool:
    return (d / ".gitignore").exists()


@directory_check
def check_has_license_mit(d: Path) -> bool:
    lic = d / "LICENSE"
    return lic.exists() and "MIT" in lic.read_text(encoding="utf-8")


@directory_check
def check_cmake_utils_renamed(d: Path) -> bool:
    cmake_dir = d / "cmake"
    if not cmake_dir.is_dir():
        return False
    if (cmake_dir / "utils.cmake").exists():
        return False
    return any(cmake_dir.glob("*-utils.cmake"))


@directory_check
def check_output_dir_is_out(d: Path) -> bool:
    cm = d / "CMakeLists.txt"
    if cm.exists() and "out" in cm.read_text(encoding="utf-8"):
        return True
    docs = d / "docs"
    if docs.is_dir():
        for f in docs.rglob("*.md"):
            if "-B out" in f.read_text(encoding="utf-8", errors="ignore"):
                return True
    return False


@directory_check
def check_lib_has_include_header(d: Path) -> bool:
    inc = d / "include"
    return inc.is_dir() and any(inc.glob("*.h"))


@directory_check
def check_lib_has_examples_dir(d: Path) -> bool:
    return (d / "examples").is_dir()


# --- Negative check ---


@text_check
def check_no_c_skill_content(text: str) -> bool:
    return not bool(
        re.search(r"CMakeLists|cmake|sanitizer|ASAN|clang-format|\.c\b.*file", text, re.I)
    )
