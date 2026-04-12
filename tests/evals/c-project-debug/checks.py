"""Deterministic checks for c-project-debug skill eval.

Each check function:
  - Named check_<name> matching the id in prompts.json expected_checks
  - Returns True (pass) or False (fail)
  - Has an 'input_type' attribute: "text" for output text
"""

import re


def text_check(fn):
    fn.input_type = "text"
    return fn


# --- Step 0: Identify failing test ---


@text_check
def check_has_ctest_identify_step(text: str) -> bool:
    """Must use ctest to identify which test fails."""
    return "ctest" in text


@text_check
def check_has_output_on_failure(text: str) -> bool:
    """ctest should use --output-on-failure."""
    return "--output-on-failure" in text


# --- Step 1: Sanitizer rebuild (always first) ---


@text_check
def check_has_sanitizer_rebuild(text: str) -> bool:
    """Must include a sanitizer rebuild step."""
    return bool(re.search(r"ENABLE_ASAN|sanitizer|fsanitize", text, re.I))


@text_check
def check_enables_asan(text: str) -> bool:
    """Must enable ASAN via CMake option."""
    return bool(re.search(r"ENABLE_ASAN[=\s]+ON", text, re.I))


@text_check
def check_enables_tsan(text: str) -> bool:
    """Must enable TSAN via CMake option."""
    return bool(re.search(r"ENABLE_TSAN[=\s]+ON", text, re.I))


@text_check
def check_cleans_out_before_configure(text: str) -> bool:
    """Must delete out/ before sanitizer reconfigure."""
    return bool(
        re.search(r"rm\s+(-rf\s+)?out", text)
        or re.search(r"rmdir\s+/s\s+/q\s+out", text, re.I)
        or re.search(r"Remove-Item.*out", text, re.I)
        or re.search(r"delete.*out", text, re.I)
    )


@text_check
def check_sanitizer_before_debugger(text: str) -> bool:
    """Sanitizer step must appear before debugger step in the output.

    The skill requires: sanitizer first, then debugger if needed.
    """
    lower = text.lower()
    san_pos = -1
    dbg_pos = -1

    for pattern in ["asan", "sanitizer", "enable_asan", "fsanitize"]:
        idx = lower.find(pattern)
        if idx != -1 and (san_pos == -1 or idx < san_pos):
            san_pos = idx

    for pattern in ["gdb", "lldb", "cdb", "windbg"]:
        idx = lower.find(pattern)
        if idx != -1 and (dbg_pos == -1 or idx < dbg_pos):
            dbg_pos = idx

    if san_pos == -1:
        return False
    if dbg_pos == -1:
        return True
    return san_pos < dbg_pos


@text_check
def check_no_asan_tsan_together(text: str) -> bool:
    """ASAN and TSAN must not be enabled simultaneously."""
    has_asan = bool(re.search(r"ENABLE_ASAN[=\s]+ON", text, re.I))
    has_tsan = bool(re.search(r"ENABLE_TSAN[=\s]+ON", text, re.I))
    return not (has_asan and has_tsan)


@text_check
def check_uses_debug_build_type(text: str) -> bool:
    """Debug builds are required for debugging."""
    return bool(re.search(r"CMAKE_BUILD_TYPE[=\s]+Debug", text, re.I))


# --- Step 2: Batch debugger (platform-specific) ---


@text_check
def check_has_gdb_batch(text: str) -> bool:
    """Must use gdb in batch mode on Linux."""
    return bool(re.search(r"gdb\s+(-batch|--batch)", text))


@text_check
def check_has_lldb_batch(text: str) -> bool:
    """Must use lldb in batch mode on macOS."""
    return bool(re.search(r"lldb\s+-b\b", text))


@text_check
def check_has_cdb_or_windbg(text: str) -> bool:
    """Must mention cdb or WinDbg for Windows debugging."""
    lower = text.lower()
    return "cdb" in lower or "windbg" in lower


@text_check
def check_has_cdb_install_guide(text: str) -> bool:
    """Must provide installation guidance for cdb on Windows."""
    lower = text.lower()
    return (
        ("windows sdk" in lower or "debugging tools" in lower
         or "install" in lower)
        and ("cdb" in lower or "windbg" in lower)
    )


@text_check
def check_no_interactive_gdb(text: str) -> bool:
    """Must not suggest interactive gdb (no bare 'gdb' without -batch)."""
    lines = text.split("\n")
    for line in lines:
        stripped = line.strip()
        if not stripped.startswith("gdb"):
            continue
        if stripped.startswith("gdb -batch") or stripped.startswith("gdb --batch"):
            continue
        if re.match(r"^gdb\s+-", stripped) and "-batch" not in stripped:
            if "-p" in stripped:
                continue
            return False
        if re.match(r"^gdb\s+out/", stripped) and "-batch" not in stripped:
            return False
    return True


@text_check
def check_no_gdb_on_windows(text: str) -> bool:
    """Must not suggest gdb on Windows."""
    lower = text.lower()
    if "gdb" not in lower:
        return True
    if "skip" in lower and "gdb" in lower:
        return True
    if "not available" in lower and "gdb" in lower:
        return True
    if "unavailable" in lower and "gdb" in lower:
        return True
    return not bool(re.search(r"gdb\s+-batch.*out[/\\]", text))


@text_check
def check_no_gdb_on_macos(text: str) -> bool:
    """Should not suggest gdb as primary debugger on macOS (use lldb)."""
    lower = text.lower()
    has_lldb = "lldb" in lower
    has_gdb_batch = bool(re.search(r"gdb\s+-batch", lower))
    if has_lldb and not has_gdb_batch:
        return True
    if has_lldb and has_gdb_batch:
        return True
    return not has_gdb_batch


# --- Step 2E: Hang debugging ---


@text_check
def check_has_attach_to_process(text: str) -> bool:
    """Must show how to attach debugger to a running/hanging process."""
    return bool(
        re.search(r"gdb.*-p\s", text)
        or re.search(r"lldb.*-p\s", text)
        or re.search(r"cdb.*-p\s", text)
        or "timeout" in text.lower()
    )


@text_check
def check_has_bt_all_threads(text: str) -> bool:
    """Must request backtrace of all threads for hang diagnosis."""
    return bool(
        re.search(r"thread apply all bt", text)
        or re.search(r"bt all", text)
        or re.search(r"~\*kb", text)
    )


@text_check
def check_mentions_common_hang_causes(text: str) -> bool:
    """Should mention common hang causes in event-loop code."""
    lower = text.lower()
    causes = [
        "safety timer", "loop_stop", "deadlock", "socket",
        "callback", "active_count", "timer", "never called",
        "never fires", "event loop",
    ]
    return sum(1 for c in causes if c in lower) >= 2


# --- Step 2F: Log-based fallback ---


@text_check
def check_has_log_based_fallback(text: str) -> bool:
    """Must mention log-based debugging as fallback."""
    lower = text.lower()
    return (
        ("fprintf" in lower or "printf" in lower or "log" in lower)
        and ("fallback" in lower or "alternative" in lower
             or "instead" in lower or "without" in lower
             or "can't install" in lower or "cannot install" in lower
             or "no debugger" in lower or "not installed" in lower)
    )


@text_check
def check_uses_fprintf_stderr(text: str) -> bool:
    """Log debugging should use fprintf(stderr, ...) not printf."""
    return "fprintf(stderr" in text or "fprintf( stderr" in text


@text_check
def check_mentions_rebuild_after_log(text: str) -> bool:
    """Must mention that C requires recompilation after adding logs."""
    lower = text.lower()
    return (
        ("rebuild" in lower or "recompil" in lower or "cmake --build" in lower)
        and ("log" in lower or "fprintf" in lower or "printf" in lower)
    )


# --- Step 3: Analyze ---


@text_check
def check_has_breakpoint_inspect(text: str) -> bool:
    """Must show how to set breakpoints and inspect variables."""
    return bool(
        re.search(r"break(point)?\s+\S+", text)
        and re.search(r"print\s+\S+", text)
    )


@text_check
def check_mentions_abort_assert(text: str) -> bool:
    """Should mention that ASSERT calls abort() for assertion failures."""
    lower = text.lower()
    return (
        ("assert" in lower or "abort" in lower)
        and ("file" in lower or "line" in lower)
    )


@text_check
def check_has_read_source_step(text: str) -> bool:
    """Should suggest reading the source code at the crash location."""
    lower = text.lower()
    return (
        "read" in lower or "look at" in lower or "inspect" in lower
        or "examine" in lower or "check" in lower or "open" in lower
        or "source" in lower or "line" in lower
    )


# --- Negative ---


@text_check
def check_no_debug_content(text: str) -> bool:
    """Unrelated prompts should not produce debug/C content."""
    return not bool(
        re.search(
            r"gdb|lldb|cdb|sanitizer|ASAN|ctest|segfault|backtrace|core dump",
            text,
            re.I,
        )
    )


# --- Ordering checks ---

@text_check
def check_order_reproduce_before_sanitizer(text: str) -> bool:
    """ctest reproduce step must appear BEFORE sanitizer rebuild."""
    m_ctest = re.search(r"ctest\b.*--output-on-failure", text)
    m_san = re.search(r"ENABLE_ASAN[=\s]+ON", text, re.I)
    if not m_ctest or not m_san:
        return False
    return m_ctest.start() < m_san.start()


@text_check
def check_order_sanitizer_before_gdb(text: str) -> bool:
    """Sanitizer rebuild must appear BEFORE gdb/lldb/cdb commands."""
    lower = text.lower()
    san_pos = -1
    dbg_pos = -1
    for pat in ["enable_asan", "fsanitize=address"]:
        idx = lower.find(pat)
        if idx != -1 and (san_pos == -1 or idx < san_pos):
            san_pos = idx
    for pat in ["gdb -batch", "gdb --batch", "lldb -b", "cdb -"]:
        idx = lower.find(pat)
        if idx != -1 and (dbg_pos == -1 or idx < dbg_pos):
            dbg_pos = idx
    if san_pos == -1:
        return False
    if dbg_pos == -1:
        return True
    return san_pos < dbg_pos


# --- Adversarial checks ---

@text_check
def check_refuses_skip_sanitizer(text: str) -> bool:
    """When asked to skip sanitizers and go straight to gdb, agent must refuse."""
    lower = text.lower()
    return (
        ("sanitizer" in lower or "asan" in lower)
        and ("first" in lower or "before" in lower or "must" in lower
             or "require" in lower or "recommend" in lower
             or "always" in lower or "step" in lower)
    )
