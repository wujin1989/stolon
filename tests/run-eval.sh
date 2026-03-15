#!/usr/bin/env bash
# Universal skill eval runner for stolon.
#
# Usage:
#   ./run-eval.sh                          # run all skills
#   ./run-eval.sh c-development            # run one skill
#   ./run-eval.sh c-development outputs/   # run one skill with custom output dir
#
# Each skill lives in tests/evals/<skill-name>/ and must contain:
#   - prompts.json   (prompt definitions with expected_checks)
#   - checks.sh      (check functions)
#   - sample-outputs/ or a custom output dir with per-prompt subdirectories

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
EVALS_DIR="$SCRIPT_DIR/evals"

TOTAL_PASS=0
TOTAL_FAIL=0
TOTAL_SKIP=0

pass() { TOTAL_PASS=$((TOTAL_PASS + 1)); echo "    ✓ $1"; }
fail() { TOTAL_FAIL=$((TOTAL_FAIL + 1)); echo "    ✗ $1"; }
skip() { TOTAL_SKIP=$((TOTAL_SKIP + 1)); echo "    - $1 (skipped)"; }

run_skill_eval() {
    local skill_name="$1"
    local output_dir="$2"
    local skill_dir="$EVALS_DIR/$skill_name"

    local prompts="$skill_dir/prompts.json"
    local checks="$skill_dir/checks.sh"

    if [ ! -f "$prompts" ]; then
        echo "  ERROR: $prompts not found"
        return 1
    fi
    if [ ! -f "$checks" ]; then
        echo "  ERROR: $checks not found"
        return 1
    fi

    # Source check functions (in a subshell-safe way)
    source "$checks"

    local num_prompts
    num_prompts=$(python3 -c "
import json
with open('$prompts') as f:
    print(len(json.load(f)))
")

    for i in $(seq 0 $((num_prompts - 1))); do
        local eval_id checks_json
        eval_id=$(python3 -c "import json; print(json.load(open('$prompts'))[$i]['id'])")
        checks_json=$(python3 -c "import json; print(' '.join(json.load(open('$prompts'))[$i]['expected_checks']))")

        echo ""
        echo "  --- $eval_id ---"

        local prompt_dir="$output_dir/$eval_id"
        local output_file="$prompt_dir/output.txt"
        local project_dir="$prompt_dir/project"

        if [ ! -d "$prompt_dir" ]; then
            skip "no output directory"
            continue
        fi

        local output_text=""
        if [ -f "$output_file" ]; then
            output_text=$(cat "$output_file")
        fi

        for check_name in $checks_json; do
            local fn="check_$check_name"

            if ! type "$fn" &>/dev/null; then
                skip "$check_name (undefined)"
                continue
            fi

            # Directory-based checks
            case "$check_name" in
                has_cmakelists|no_unreplaced_placeholders|cmake_has_project_options|\
                cmake_c_standard_11|has_clang_format|has_gitignore|has_license_mit|\
                cmake_utils_renamed|output_dir_is_out|lib_has_include_header|\
                lib_has_examples_dir|source_file_has_project_prefix)
                    if [ -d "$project_dir" ]; then
                        if $fn "$project_dir"; then pass "$check_name"; else fail "$check_name"; fi
                    else
                        skip "$check_name (no project dir)"
                    fi
                    ;;
                *)
                    if [ -n "$output_text" ]; then
                        if $fn "$output_text"; then pass "$check_name"; else fail "$check_name"; fi
                    else
                        skip "$check_name (no output)"
                    fi
                    ;;
            esac
        done
    done
}

# ─── Main ───

SKILL_FILTER="${1:-}"
CUSTOM_OUTPUT="${2:-}"

if [ -n "$SKILL_FILTER" ]; then
    skills=("$SKILL_FILTER")
else
    skills=()
    for d in "$EVALS_DIR"/*/; do
        [ -d "$d" ] && skills+=("$(basename "$d")")
    done
fi

for skill in "${skills[@]}"; do
    echo ""
    echo "═══ Skill: $skill ═══"

    if [ -n "$CUSTOM_OUTPUT" ]; then
        out_dir="$CUSTOM_OUTPUT"
    else
        out_dir="$EVALS_DIR/$skill/sample-outputs"
    fi

    run_skill_eval "$skill" "$out_dir"
done

echo ""
echo "==============================="
echo "  PASSED:  $TOTAL_PASS"
echo "  FAILED:  $TOTAL_FAIL"
echo "  SKIPPED: $TOTAL_SKIP"
echo "==============================="

if [ "$TOTAL_FAIL" -gt 0 ]; then
    exit 1
fi
