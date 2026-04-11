"""Deterministic checks for c-project-build skill eval.

Each check function:
  - Named check_<name> matching the id in prompts.json expected_checks
  - Returns True (pass) or False (fail)
  - Has an 'input_type' attribute: "text" for output text
"""

import re


def text_check(fn):
    fn.input_type = "text"
    return fn


# --- Core build commands ---


@text_check
def check_uses_ninja_generator(text: str) -> bool:
    """Output must specify Ninja as the CMake generator."""
    return bool(re.search(r"-G\s*Ninja", text))


@text_check
def check_uses_cmake_b_out(text: str) -> bool:
    """Output must use -B out as the build directory."""
    return bool(re.search(r"-B\s+out\b", text))


@text_check
def check_build_type_debug(text: str) -> bool:
    """Output must set CMAKE_BUILD_TYPE=Debug."""
    return bool(re.search(r"CMAKE_BUILD_TYPE[=\s]+Debug", text, re.I))


@text_check
def check_build_type_release(text: str) -> bool:
    """Output must set CMAKE_BUILD_TYPE=Release."""
    return bool(re.search(r"CMAKE_BUILD_TYPE[=\s]+Release\b", text, re.I))


@text_check
def check_build_type_minsizerel(text: str) -> bool:
    """Output must set CMAKE_BUILD_TYPE=MinSizeRel."""
    return bool(re.search(r"CMAKE_BUILD_TYPE[=\s]+MinSizeRel", text, re.I))


@text_check
def check_has_build_command(text: str) -> bool:
    """Output must include cmake --build."""
    return "cmake --build" in text


@text_check
def check_has_parallel_jobs(text: str) -> bool:
    """Build command should use -j for parallel compilation."""
    return bool(re.search(r"-j\s*[\d$({]", text))


@text_check
def check_copies_compile_commands(text: str) -> bool:
    """Output should mention copying compile_commands.json to project root."""
    return "compile_commands.json" in text


@text_check
def check_no_msbuild_or_visual_studio(text: str) -> bool:
    """Should not suggest MSBuild or Visual Studio generators on Unix."""
    lower = text.lower()
    return "msbuild" not in lower and '"visual studio' not in lower


# --- Clean before configure ---


@text_check
def check_cleans_out_before_configure(text: str) -> bool:
    """Must delete out/ before running cmake configure."""
    return bool(
        re.search(r"rm\s+(-rf\s+)?out", text)
        or re.search(r"rmdir\s+/s\s+/q\s+out", text, re.I)
        or re.search(r"Remove-Item.*out", text, re.I)
        or re.search(r"delete.*out", text, re.I)
    )


# --- Four build types ---


@text_check
def check_asks_build_type(text: str) -> bool:
    """When user doesn't specify build type, agent must ask."""
    lower = text.lower()
    return (
        "which" in lower or "what" in lower or "choose" in lower
        or "select" in lower or "?" in text
    )


@text_check
def check_presents_four_build_types(text: str) -> bool:
    """Agent must present all four CMake build types."""
    lower = text.lower()
    return (
        "debug" in lower
        and "release" in lower
        and "minsizerel" in lower
        and ("relwithdebinfo" in lower or "rel with deb" in lower)
    )


# --- Test commands ---


@text_check
def check_has_ctest_command(text: str) -> bool:
    """Output must include ctest."""
    return "ctest" in text


@text_check
def check_has_output_on_failure(text: str) -> bool:
    """ctest should use --output-on-failure."""
    return "--output-on-failure" in text


@text_check
def check_has_ctest_r_filter(text: str) -> bool:
    """Single-test run should use ctest -R to filter."""
    return bool(re.search(r"ctest\b.*-R", text))


# --- Sanitizers ---


@text_check
def check_enables_asan(text: str) -> bool:
    """Output must enable ASAN via CMake option."""
    return bool(re.search(r"ENABLE_ASAN[=\s]+ON", text, re.I))


@text_check
def check_no_tsan_with_asan(text: str) -> bool:
    """ASAN and TSAN must not be enabled simultaneously."""
    has_asan = bool(re.search(r"ENABLE_ASAN[=\s]+ON", text, re.I))
    has_tsan = bool(re.search(r"ENABLE_TSAN[=\s]+ON", text, re.I))
    return not (has_asan and has_tsan)


@text_check
def check_no_asan_tsan_together(text: str) -> bool:
    """Output should warn that ASAN and TSAN cannot coexist."""
    lower = text.lower()
    return (
        ("cannot" in lower or "conflict" in lower
         or "simultaneously" in lower or "same time" in lower
         or "incompatible" in lower or "not" in lower)
        and ("asan" in lower or "address" in lower)
        and ("tsan" in lower or "thread" in lower)
    )


# --- Coverage ---


@text_check
def check_enables_coverage(text: str) -> bool:
    """Output must enable coverage via CMake option."""
    return bool(re.search(r"ENABLE_COVERAGE[=\s]+ON", text, re.I))


@text_check
def check_has_coverage_target(text: str) -> bool:
    """Output must include the coverage build target."""
    return "--target coverage" in text


@text_check
def check_enables_testing(text: str) -> bool:
    """Coverage requires testing to be enabled."""
    return bool(re.search(r"ENABLE_TESTING[=\s]+ON", text, re.I))


# --- Reconfigure ---


@text_check
def check_mentions_clean_rebuild(text: str) -> bool:
    """Switching sanitizers requires a clean rebuild."""
    lower = text.lower()
    return (
        "clean" in lower
        or "fresh" in lower
        or "scratch" in lower
        or "delete" in lower
        or "remove" in lower
        or "rm " in lower
    )


@text_check
def check_mentions_rm_out(text: str) -> bool:
    """Should mention removing the out directory."""
    return bool(
        re.search(r"rm\s+(-rf\s+)?out", text)
        or re.search(r"rmdir\s+/s\s+/q\s+out", text, re.I)
        or re.search(r"Remove-Item.*out", text, re.I)
    )


# --- TLS ---


@text_check
def check_no_tls_enabled(text: str) -> bool:
    """When building without TLS, must not enable TLS option."""
    return not bool(re.search(r"ENABLE_TLS[=\s]+ON", text, re.I))


# --- Negative ---


@text_check
def check_no_cmake_content(text: str) -> bool:
    """Unrelated prompts should not produce CMake/build content."""
    return not bool(
        re.search(r"cmake|ninja|ctest|sanitizer|ASAN|coverage", text, re.I)
    )
