#!/usr/bin/env bash
# Deterministic check functions for skill eval.
# Each function receives the agent output (file or text) and returns 0 (pass) or 1 (fail).
# Usage: source this file, then call check_<name> "$output_dir" or "$output_text"

# ─── Setup checks: verify generated project structure ───

check_has_cmakelists() {
    local dir="$1"
    [ -f "$dir/CMakeLists.txt" ]
}

check_no_unreplaced_placeholders() {
    local dir="$1"
    # Search all files for raw {project}, {PROJECT}, {YEAR}, {AUTHOR}, {EMAIL}, {DESCRIPTION}
    ! grep -riP '\{(project|year|author|email|description)\}' \
        --include='*.c' --include='*.h' --include='*.cmake' \
        --include='*.txt' --include='*.md' \
        "$dir" 2>/dev/null
}

check_cmake_has_project_options() {
    local dir="$1"
    local cm="$dir/CMakeLists.txt"
    [ -f "$cm" ] && \
    grep -q 'ENABLE_TESTING' "$cm" && \
    grep -q 'ENABLE_ASAN' "$cm" && \
    grep -q 'ENABLE_TSAN' "$cm" && \
    grep -q 'ENABLE_UBSAN' "$cm"
}

check_cmake_c_standard_11() {
    local dir="$1"
    grep -q 'CMAKE_C_STANDARD 11' "$dir/CMakeLists.txt" 2>/dev/null
}

check_has_clang_format() {
    local dir="$1"
    [ -f "$dir/.clang-format" ]
}

check_has_gitignore() {
    local dir="$1"
    [ -f "$dir/.gitignore" ]
}

check_has_license_mit() {
    local dir="$1"
    [ -f "$dir/LICENSE" ] && grep -qi 'MIT' "$dir/LICENSE"
}

check_cmake_utils_renamed() {
    local dir="$1"
    # utils.cmake should NOT exist; {project}-utils.cmake should
    [ ! -f "$dir/cmake/utils.cmake" ] && \
    ls "$dir/cmake/"*-utils.cmake >/dev/null 2>&1
}

check_output_dir_is_out() {
    local dir="$1"
    # CMakeLists.txt or docs should reference 'out/' as build dir
    grep -q 'out' "$dir/CMakeLists.txt" 2>/dev/null || \
    grep -rq '\-B out' "$dir/docs/" 2>/dev/null
}

check_lib_has_include_header() {
    local dir="$1"
    ls "$dir/include/"*.h >/dev/null 2>&1
}

check_lib_has_examples_dir() {
    local dir="$1"
    [ -d "$dir/examples" ]
}

# ─── Style checks: verify naming conventions in generated code ───

check_func_name_has_project_prefix() {
    local text="$1"
    # Public functions should match pattern: <project>_<module>_<action>
    # At minimum, there should be a function with underscore-separated prefix
    echo "$text" | grep -qP '^\w+_\w+_\w+\s*\(' 2>/dev/null
}

check_source_file_has_project_prefix() {
    local dir="$1"
    # Source files in src/ should be named {project}-<module>.c
    ls "$dir/src/"*-*.c >/dev/null 2>&1
}

check_header_has_license_block() {
    local text="$1"
    echo "$text" | head -1 | grep -q 'Copyright'
}

check_header_has_pragma_once() {
    local text="$1"
    echo "$text" | grep -q '_Pragma("once")'
}

check_type_name_has_project_prefix() {
    local text="$1"
    # Types should match: <project>_<module>_t
    echo "$text" | grep -qP '\w+_\w+_t\b'
}

check_static_func_underscore_prefix() {
    local text="$1"
    # Static functions should start with _<name>
    echo "$text" | grep -qP 'static\s+\w+\s+_\w+\s*\('
}

check_no_project_prefix_on_static() {
    local text="$1"
    # Static (internal) functions should NOT have the project prefix
    # This is a heuristic: static void <project>_xxx should not appear
    ! echo "$text" | grep -qP 'static\s+\w+\s+\w+_\w+_\w+_\w+\s*\('
}

# ─── Build checks: verify CMake patterns ───

check_source_in_srcs_list() {
    local text="$1"
    echo "$text" | grep -q 'SRCS'
}

check_no_cmake_c_flags() {
    local text="$1"
    ! echo "$text" | grep -q 'CMAKE_C_FLAGS'
}

check_no_hardcoded_paths() {
    local text="$1"
    # No absolute paths like /usr/lib or C:\
    ! echo "$text" | grep -qP '(/usr/|/opt/|C:\\|D:\\)'
}

# ─── Test checks: verify test file conventions ───

check_test_uses_project_assert() {
    local text="$1"
    echo "$text" | grep -q '#include "assert.h"'
}

check_test_no_stdlib_assert() {
    local text="$1"
    ! echo "$text" | grep -q '#include <assert.h>'
}

check_test_has_main() {
    local text="$1"
    echo "$text" | grep -qP 'int\s+main\s*\('
}

check_test_func_is_static_void() {
    local text="$1"
    echo "$text" | grep -qP 'static\s+void\s+test_\w+\s*\('
}

check_test_registered_with_add_test() {
    local text="$1"
    echo "$text" | grep -qP '\w+_add_test\s*\('
}

check_test_file_named_correctly() {
    local text="$1"
    # Should reference test-<module>.c pattern
    echo "$text" | grep -qP 'test-\w+\.c'
}

check_test_no_file_scope_mutable() {
    local text="$1"
    # No non-const file-scope variables (rough heuristic: global non-static-func declarations)
    # This checks that there are no lines like: int global_var = ...;  outside functions
    # Simplified: just check there's no obvious global mutable state
    ! echo "$text" | grep -qP '^\s*(int|char|void\s*\*|size_t|uint\w+)\s+\w+\s*=' 2>/dev/null
}

check_test_each_func_has_init_deinit() {
    local text="$1"
    # Each test function should have both init and deinit/destroy/close calls
    echo "$text" | grep -qP '(init|create)\s*\(' && \
    echo "$text" | grep -qP '(deinit|destroy|close|free)\s*\('
}

check_test_no_sleep() {
    local text="$1"
    ! echo "$text" | grep -qP '(sleep|thrd_sleep)\s*\('
}

# ─── Sanitizer/debug checks: verify advice content ───

check_mentions_enable_asan_option() {
    local text="$1"
    echo "$text" | grep -qi 'ENABLE_ASAN'
}

check_uses_cmake_option_not_flags() {
    local text="$1"
    # Should recommend -D..._ENABLE_ASAN=ON (the CMake option approach)
    # Agent may mention -fsanitize in a "don't do this" context, so we only
    # check that the CMake option IS recommended, not that -fsanitize is absent.
    echo "$text" | grep -qP '\-D\w+_ENABLE_ASAN'
}

check_does_not_combine_asan_tsan() {
    local text="$1"
    # Should not suggest enabling both simultaneously
    ! echo "$text" | grep -qiP 'ENABLE_ASAN.*ENABLE_TSAN|ENABLE_TSAN.*ENABLE_ASAN'
}

check_suggests_asan_first() {
    local text="$1"
    echo "$text" | grep -qi 'asan\|addresssanitizer\|address.sanitizer'
}

check_mentions_long_size_difference() {
    local text="$1"
    echo "$text" | grep -qiP 'long.*(4|8)\s*byte|int32_t|int64_t'
}

check_mentions_stack_size_difference() {
    local text="$1"
    echo "$text" | grep -qiP 'stack.*(1\s*MB|8\s*MB|overflow)'
}

check_follows_diagnosis_order() {
    local text="$1"
    # Should mention reading error first, then sanitizer, then debugger
    echo "$text" | grep -qi 'sanitizer\|asan'
}

# ─── Deploy checks ───

check_mentions_test_pass() {
    local text="$1"
    echo "$text" | grep -qiP 'test.*pass|ctest|all tests'
}

check_mentions_asan_clean() {
    local text="$1"
    echo "$text" | grep -qi 'asan'
}

check_mentions_ubsan_clean() {
    local text="$1"
    echo "$text" | grep -qi 'ubsan'
}

check_mentions_no_placeholders() {
    local text="$1"
    echo "$text" | grep -qiP 'placeholder|unreplaced|\{project\}'
}

check_mentions_version_in_cmake() {
    local text="$1"
    echo "$text" | grep -qiP 'project\s*\(.*VERSION|CMakeLists'
}

check_uses_semver() {
    local text="$1"
    echo "$text" | grep -qiP 'semver|semantic.version|\d+\.\d+\.\d+'
}

# ─── Negative check ───

check_no_c_skill_content() {
    local text="$1"
    # Should NOT contain C-skill-specific terms
    ! echo "$text" | grep -qiP 'CMakeLists|cmake|sanitizer|ASAN|clang-format|\.c\b.*file'
}
