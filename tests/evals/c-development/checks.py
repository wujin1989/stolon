"""Deterministic checks for c-development skill eval.

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


# ─── Setup checks: generated project structure ───


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
    # utils.cmake should NOT exist; *-utils.cmake should
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


@directory_check
def check_source_file_has_project_prefix(d: Path) -> bool:
    src = d / "src"
    return src.is_dir() and any(src.glob("*-*.c"))


# ─── Style checks: naming conventions in generated code ───


@text_check
def check_func_name_has_project_prefix(text: str) -> bool:
    return bool(re.search(r"\w+_\w+_\w+\s*\(", text))


@text_check
def check_header_has_license_block(text: str) -> bool:
    return "Copyright" in text.split("\n")[0] if text else False


@text_check
def check_header_has_pragma_once(text: str) -> bool:
    return '_Pragma("once")' in text


@text_check
def check_type_name_has_project_prefix(text: str) -> bool:
    return bool(re.search(r"\w+_\w+_t\b", text))


@text_check
def check_static_func_underscore_prefix(text: str) -> bool:
    return bool(re.search(r"static\s+\w+\s+_\w+\s*\(", text))


@text_check
def check_no_project_prefix_on_static(text: str) -> bool:
    return not bool(re.search(r"static\s+\w+\s+\w+_\w+_\w+_\w+\s*\(", text))


# ─── Build checks ───


@text_check
def check_source_in_srcs_list(text: str) -> bool:
    return "SRCS" in text


@text_check
def check_no_cmake_c_flags(text: str) -> bool:
    return "CMAKE_C_FLAGS" not in text


@text_check
def check_no_hardcoded_paths(text: str) -> bool:
    return not bool(re.search(r"(/usr/|/opt/|C:\\|D:\\)", text))


# ─── Test checks ───


@text_check
def check_test_uses_project_assert(text: str) -> bool:
    return '#include "assert.h"' in text


@text_check
def check_test_no_stdlib_assert(text: str) -> bool:
    return "#include <assert.h>" not in text


@text_check
def check_test_has_main(text: str) -> bool:
    return bool(re.search(r"int\s+main\s*\(", text))


@text_check
def check_test_func_is_static_void(text: str) -> bool:
    return bool(re.search(r"static\s+void\s+test_\w+\s*\(", text))


@text_check
def check_test_registered_with_add_test(text: str) -> bool:
    return bool(re.search(r"\w+_add_test\s*\(", text))


@text_check
def check_test_file_named_correctly(text: str) -> bool:
    return bool(re.search(r"test-\w+\.c", text))


@text_check
def check_test_no_file_scope_mutable(text: str) -> bool:
    return not bool(
        re.search(r"^\s*(int|char|void\s*\*|size_t|uint\w+)\s+\w+\s*=", text, re.M)
    )


@text_check
def check_test_each_func_has_init_deinit(text: str) -> bool:
    has_init = bool(re.search(r"(init|create)\s*\(", text))
    has_deinit = bool(re.search(r"(deinit|destroy|close|free)\s*\(", text))
    return has_init and has_deinit


@text_check
def check_test_no_sleep(text: str) -> bool:
    return not bool(re.search(r"(sleep|thrd_sleep)\s*\(", text))


# ─── Sanitizer / debug checks ───


@text_check
def check_mentions_enable_asan_option(text: str) -> bool:
    return "ENABLE_ASAN" in text.upper()


@text_check
def check_uses_cmake_option_not_flags(text: str) -> bool:
    return bool(re.search(r"-D\w+_ENABLE_ASAN", text))


@text_check
def check_does_not_combine_asan_tsan(text: str) -> bool:
    return not bool(
        re.search(r"ENABLE_ASAN.*ENABLE_TSAN|ENABLE_TSAN.*ENABLE_ASAN", text, re.I)
    )


@text_check
def check_suggests_asan_first(text: str) -> bool:
    return bool(re.search(r"asan|addresssanitizer|address.sanitizer", text, re.I))


@text_check
def check_mentions_long_size_difference(text: str) -> bool:
    return bool(re.search(r"long.*(4|8)\s*byte|int32_t|int64_t", text, re.I))


@text_check
def check_mentions_stack_size_difference(text: str) -> bool:
    return bool(re.search(r"stack.*(1\s*MB|8\s*MB|overflow)", text, re.I))


@text_check
def check_follows_diagnosis_order(text: str) -> bool:
    return bool(re.search(r"sanitizer|asan", text, re.I))


# ─── Deploy checks ───


@text_check
def check_mentions_test_pass(text: str) -> bool:
    return bool(re.search(r"test.*pass|ctest|all tests", text, re.I))


@text_check
def check_mentions_asan_clean(text: str) -> bool:
    return bool(re.search(r"asan", text, re.I))


@text_check
def check_mentions_ubsan_clean(text: str) -> bool:
    return bool(re.search(r"ubsan", text, re.I))


@text_check
def check_mentions_no_placeholders(text: str) -> bool:
    return bool(re.search(r"placeholder|unreplaced|\{project\}", text, re.I))


@text_check
def check_mentions_version_in_cmake(text: str) -> bool:
    return bool(re.search(r"project\s*\(.*VERSION|CMakeLists", text, re.I))


@text_check
def check_uses_semver(text: str) -> bool:
    return bool(re.search(r"semver|semantic.version|\d+\.\d+\.\d+", text, re.I))


# ─── Negative check ───


@text_check
def check_no_c_skill_content(text: str) -> bool:
    return not bool(
        re.search(r"CMakeLists|cmake|sanitizer|ASAN|clang-format|\.c\b.*file", text, re.I)
    )
