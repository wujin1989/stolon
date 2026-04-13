# Build Workflow

## Overview

Reference guide for the build/test/coverage cycle of C (C11+) CMake projects. All commands assume the project root contains a `CMakeLists.txt` with a valid `project()` call.

Placeholders:
- `{name}` -- lowercase project name from `CMakeLists.txt`
- `{NAME}` -- uppercase form
- `{build_dir}` -- CMake build directory (default: `out`). Detect from existing `cmake -B` invocations or `CMakeCache.txt` location. All commands below use `out` as the default; substitute the actual directory if different.

## Inputs — MANDATORY Checks Before Build

**STOP. Do NOT run any cmake command until you have collected ALL required inputs.**

| Input | How to obtain | Default |
|-------|---------------|---------|
| Project name | Read `project(...)` from root `CMakeLists.txt` | (required) |
| Build type | **Ask user**: Debug / Release / RelWithDebInfo / MinSizeRel | **(required, must ask)** |
| Feature flags | **Ask user** which optional features to enable | None |

Feature flags are project-specific `option()` entries in `CMakeLists.txt` (e.g. `{NAME}_ENABLE_TLS`). Scan the root `CMakeLists.txt` for all `option(...)` lines and present them to the user. Only enable flags the user explicitly requests.

Sanitizer and coverage are **not** part of the standard checks. Only ask about them when the user explicitly mentions sanitizer, ASAN, TSAN, UBSAN, or coverage in their request.

**An existing `out/` directory is NOT a reason to skip asking.** The cached config may be stale or wrong. Always confirm with the user before configuring or building.

**Red flags — if you catch yourself doing any of these, STOP:**
- "out/ exists, I'll just build" → STOP. Ask the user first.
- "It was Debug last time, probably still Debug" → STOP. Confirm.
- "I'll enable TLS because it was on before" → STOP. Ask.
- "User said compile, they just want a quick build" → STOP. Quick ≠ skip confirmation.

## Build Type

| Build type | `--config` value | Use case |
|------------|--------------------|----------|
| Debug | `Debug` | Development, testing, sanitizers, coverage |
| Release | `Release` | Speed optimization (`-O2` / `/O2`) |
| RelWithDebInfo | `RelWithDebInfo` | Speed optimization + debug info |
| MinSizeRel | `MinSizeRel` | Size optimization (`-Os` / `/O1`) |

## Platform Strategy

All platforms use Ninja Multi-Config. Build type is selected at build time via `--config {build_type}`, not at configure time. This ensures `CMAKE_EXPORT_COMPILE_COMMANDS` produces `compile_commands.json`. Visual Studio generators do not produce this file.

### Windows MSVC Environment Activation

On Windows, Ninja and MSVC compiler require the Visual Studio developer environment. The reliable approach is to wrap **every** cmake/ctest command in a `cmd /c` call that first activates the environment:

**Step 1 — Locate `vcvarsall.bat`:**

```powershell
$vsPath = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath 2>$null
# vcvarsall.bat is at: "$vsPath\VC\Auxiliary\Build\vcvarsall.bat"
```

Cache the full path to `vcvarsall.bat` for the session. If `vswhere.exe` is not found, tell the user to install Visual Studio Build Tools.

**Step 2 — Run cmake commands via `cmd /c`:**

All Windows cmake/ctest commands in this document MUST be executed using this pattern:

```powershell
cmd /c '"{vcvarsall}" x64 >nul 2>&1 && {cmake_command}'
```

Where `{vcvarsall}` is the full path from Step 1. Example:

```powershell
cmd /c '"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1 && cmake -B out -G "Ninja Multi-Config" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON'
```

**Important:** The `cmd /c` wrapper causes PowerShell to report exit code -1 even on success. Do NOT treat exit code -1 as a build failure. Instead, check the command output for actual errors (e.g. `FAILED:`, `error C`, `error LNK`, `CMake Error`).

## compile_commands.json

After every successful configure or build, copy `compile_commands.json` from the build directory to the project root so clangd can find it:

| Platform | Command |
|----------|---------|
| Unix | `cp -f out/compile_commands.json compile_commands.json 2>/dev/null \|\| true` |
| Windows | `Copy-Item -Force out\compile_commands.json compile_commands.json -ErrorAction SilentlyContinue` |

This file should be in `.gitignore`.

## Commands

### Configure

**MANDATORY: Always delete `out/` before configuring.** Stale cache can produce incorrect builds.

```bash
rm -rf out   # Unix
```
```powershell
if (Test-Path out) { Remove-Item -Recurse -Force out }   # Windows
```

#### Unix (Linux / macOS)

```bash
cmake -B out -G "Ninja Multi-Config" \
  [-D{NAME}_ENABLE_TESTING=ON] \
  [-D{NAME}_ENABLE_{FEATURE}=ON]
```

#### Windows (MSVC + Ninja Multi-Config)

Use the `cmd /c` + `vcvarsall.bat` pattern described in **Windows MSVC Environment Activation**. All flags go on a single line inside the quoted command string:

```powershell
cmd /c '"{vcvarsall}" x64 >nul 2>&1 && cmake -B out -G "Ninja Multi-Config" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON [-D{NAME}_ENABLE_TESTING=ON] [-D{NAME}_ENABLE_{FEATURE}=ON]'
```

### Build

Unix:

```bash
cmake --build out --config {build_type} -j {ncpu}
```

On Unix with Release/RelWithDebInfo/MinSizeRel, strip debug symbols after build:

```bash
strip out/{build_type}/{target}      # shared lib / executable
strip -g out/{build_type}/lib{name}.a  # static lib: strip debug only, keep symbols
```

Windows (via `cmd /c` wrapper):

```powershell
cmd /c '"{vcvarsall}" x64 >nul 2>&1 && cmake --build out --config {build_type} -j {ncpu}'
```

Detect CPU count:
- Linux: `nproc`
- macOS: `sysctl -n hw.ncpu`
- Windows: `%NUMBER_OF_PROCESSORS%` or default `8`
- Fallback: `4`

### Test

Unix:

```bash
ctest --test-dir out -C {build_type} --output-on-failure
```

Windows (via `cmd /c` wrapper):

```powershell
cmd /c '"{vcvarsall}" x64 >nul 2>&1 && ctest --test-dir out -C {build_type} --output-on-failure'
```

Run a single test module (same pattern, add `-R {module}`):

```bash
ctest --test-dir out -C {build_type} -R {module} --output-on-failure        # Unix
```
```powershell
cmd /c '"{vcvarsall}" x64 >nul 2>&1 && ctest --test-dir out -C {build_type} -R {module} --output-on-failure'   # Windows
```

### Sanitizers

Enable one sanitizer at a time. ASAN and TSAN cannot coexist.

Unix:

```bash
cmake -B out -G "Ninja Multi-Config" \
  -D{NAME}_ENABLE_TESTING=ON \
  -D{NAME}_ENABLE_ASAN=ON

cmake --build out --config Debug -j {ncpu}
ctest --test-dir out -C Debug --output-on-failure
```

Windows (via `cmd /c` wrapper):

```powershell
cmd /c '"{vcvarsall}" x64 >nul 2>&1 && cmake -B out -G "Ninja Multi-Config" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -D{NAME}_ENABLE_TESTING=ON -D{NAME}_ENABLE_ASAN=ON'

cmd /c '"{vcvarsall}" x64 >nul 2>&1 && cmake --build out --config Debug -j {ncpu}'
cmd /c '"{vcvarsall}" x64 >nul 2>&1 && ctest --test-dir out -C Debug --output-on-failure'
```

| Sanitizer | Option | Catches |
|-----------|--------|---------|
| ASAN | `-D{NAME}_ENABLE_ASAN=ON` | Buffer overflow, use-after-free, memory leaks |
| TSAN | `-D{NAME}_ENABLE_TSAN=ON` | Data races, deadlocks |
| UBSAN | `-D{NAME}_ENABLE_UBSAN=ON` | Undefined behavior (signed overflow, null deref, etc.) |

UBSAN can be combined with ASAN or TSAN.

### Coverage

#### Unix

```bash
cmake -B out -G "Ninja Multi-Config" \
  -D{NAME}_ENABLE_TESTING=ON \
  -D{NAME}_ENABLE_COVERAGE=ON

cmake --build out --config Debug -j {ncpu}
cmake --build out --config Debug --target coverage
```

Requires `lcov` and `genhtml`. HTML report at `out/coverage/html/index.html`.

#### Windows

Windows coverage uses OpenCppCoverage (runtime instrumentation, no compiler flags needed). The `{NAME}_ENABLE_COVERAGE=ON` flag is needed so CMake creates the `coverage` target.

```powershell
cmd /c '"{vcvarsall}" x64 >nul 2>&1 && cmake -B out -G "Ninja Multi-Config" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -D{NAME}_ENABLE_TESTING=ON -D{NAME}_ENABLE_COVERAGE=ON'

cmd /c '"{vcvarsall}" x64 >nul 2>&1 && cmake --build out --config Debug -j 8'
cmd /c '"{vcvarsall}" x64 >nul 2>&1 && cmake --build out --config Debug --target coverage'
```

HTML report at `out/coverage/index.html`.

If `cmake --build out --target coverage` fails with "no rule to make target 'coverage'", OpenCppCoverage is not installed or not on PATH. Install from https://github.com/OpenCppCoverage/OpenCppCoverage/releases, ensure on PATH, then reconfigure. Also check `tests/CMakeLists.txt` for the Windows coverage target — it may not be in the root CMakeLists.

## Reconfigure vs Rebuild

| Scenario | Action |
|----------|--------|
| Changed source files only | Build only (cmake --build out) |
| Any other change (options, CMakeLists.txt, sanitizer) | Delete `out/` and reconfigure from scratch |

**Rule: Every configure = delete `out/` first.** This prevents stale cache from producing incorrect builds.

Delete `out/` (see Configure section), then configure + build.

