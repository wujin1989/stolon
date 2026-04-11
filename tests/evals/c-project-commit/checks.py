"""Deterministic checks for c-project-commit skill eval.

Each check function:
  - Named check_<name> matching the id in prompts.json expected_checks
  - Returns True (pass) or False (fail)
  - Has an 'input_type' attribute: "text" for output text
"""

import re


def text_check(fn):
    fn.input_type = "text"
    return fn


# --- Pre-commit validation steps ---


@text_check
def check_has_build_step(text: str) -> bool:
    """Must include a build step before committing."""
    return "cmake --build" in text


@text_check
def check_has_test_step(text: str) -> bool:
    """Must include running tests before committing."""
    return "ctest" in text


@text_check
def check_has_sanitizer_step(text: str) -> bool:
    """Must include a sanitizer check before committing."""
    return bool(re.search(r"ENABLE_ASAN|ENABLE_UBSAN|ENABLE_TSAN|sanitizer", text, re.I))


@text_check
def check_sanitizer_uses_asan(text: str) -> bool:
    """Pre-commit sanitizer should include ASAN."""
    return bool(re.search(r"ENABLE_ASAN[=\s]+ON", text, re.I))


@text_check
def check_sanitizer_uses_ubsan(text: str) -> bool:
    """Pre-commit sanitizer should include UBSAN."""
    return bool(re.search(r"ENABLE_UBSAN[=\s]+ON", text, re.I))


# --- Git workflow ---


@text_check
def check_has_git_diff_or_status(text: str) -> bool:
    """Must review changes before staging."""
    return "git diff" in text or "git status" in text


@text_check
def check_has_git_add(text: str) -> bool:
    """Must stage files."""
    return "git add" in text


@text_check
def check_no_blind_git_add_dot(text: str) -> bool:
    """Should not blindly use 'git add .' without review warning."""
    if "git add ." not in text and "git add -A" not in text:
        return True
    lower = text.lower()
    return (
        "review" in lower
        or "careful" in lower
        or "untracked" in lower
        or "status" in lower
        or "git add -u" in text
    )


@text_check
def check_has_git_commit(text: str) -> bool:
    """Must include git commit."""
    return "git commit" in text


@text_check
def check_has_git_push(text: str) -> bool:
    """Must include git push."""
    return "git push" in text


# --- Commit message format ---


@text_check
def check_commit_msg_conventional(text: str) -> bool:
    """Commit message must follow conventional format: type(scope): summary."""
    return bool(
        re.search(
            r'(feat|fix|refactor|test|docs|chore|perf)\([a-z]+\):\s',
            text,
        )
    )


@text_check
def check_commit_msg_imperative(text: str) -> bool:
    """Commit summary should use imperative mood (add, not added/adds)."""
    match = re.search(
        r'(feat|fix|refactor|test|docs|chore|perf)\([a-z]+\):\s+(\w+)',
        text,
    )
    if not match:
        return False
    verb = match.group(2).lower()
    return not verb.endswith("ed") and not verb.endswith("ing")


@text_check
def check_commit_msg_has_scope(text: str) -> bool:
    """Commit message should include a scope (module name)."""
    return bool(
        re.search(
            r'(feat|fix|refactor|test|docs|chore|perf)\([a-z]+\)',
            text,
        )
    )


@text_check
def check_commit_type_test(text: str) -> bool:
    """Test-only changes should use 'test' type."""
    return bool(re.search(r'\btest\(', text))


# --- Commit message explanation ---


@text_check
def check_mentions_type_scope_summary(text: str) -> bool:
    """Should explain the type(scope): summary format."""
    lower = text.lower()
    return "type" in lower and "scope" in lower and "summary" in lower


@text_check
def check_mentions_imperative_mood(text: str) -> bool:
    """Should mention imperative mood for commit summaries."""
    lower = text.lower()
    return "imperative" in lower


@text_check
def check_lists_commit_types(text: str) -> bool:
    """Should list the valid commit types."""
    return (
        "feat" in text
        and "fix" in text
        and "refactor" in text
        and "test" in text
    )


@text_check
def check_mentions_scope_is_module(text: str) -> bool:
    """Should explain that scope is the module name."""
    lower = text.lower()
    return "module" in lower and "scope" in lower


# --- Negative ---


@text_check
def check_no_commit_content(text: str) -> bool:
    """Unrelated prompts should not produce commit/git content."""
    return not bool(
        re.search(
            r"git commit|git push|conventional commit|ASAN|ctest",
            text,
            re.I,
        )
    )
