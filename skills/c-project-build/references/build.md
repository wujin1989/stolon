# Build Workflow

## Overview

This skill drives the build/test/coverage cycle for C11 CMake projects. All commands assume the project root contains a `CMakeLists.txt` with a valid `project()` call.

Placeholders:
- `{name}` -- lowercase project name from `CMakeLists.txt`
- `{NAME}` -- uppercase form

## Inputs — MANDATORY Pre-Flight

**STOP. Do NOT run any cmake command until you have collected ALL required inputs.**

| Input | How to obtain | Default |
|-------|---------------|---------|
| Project name | Read `project(...)` from root `CMakeLists.txt` | (required) |
| Build type | **Ask user**: Debug / Release / RelWithDebInfo / MinSizeRel | **(required, must ask)** |
| Feature flags | **Ask user** which optional features to enable | None |
| Sanitizer | **Ask user** if needed | None |
| Coverage | **Ask user** if needed | OFF |

Feature flags are project-specific `option()` entries in `CMakeLists.txt` (e.g. `{NAME}_ENABLE_TLS`). Scan the root `CMakeLists.txt` for all `option(...)` lines and present them to the user. Only enable flags the user explicitly requests.

**An existing `out/` directory is NOT a reason to skip asking.** The cached config may be stale or wrong. Always confirm with the user before configuring or building.

**Red flags — if you catch yourself doing any of these, STOP:**
- "out/ exists, I'll just build" → STOP. Ask the user first.
- "It was Debug last time, probably still Debug" → STOP. Confirm.
- "I'll enable TLS because it was on before" → STOP. Ask.
- "User said compile, they just want a quick build" → STOP. Quick ≠ skip confirmation.

## Build Type

Always ask the user which of the four CMake build types to use before configuring:

| Build type | `CMAKE_BUILD_TYPE` | Use case |
|------------|--------------------|----------|
| Debug | `Debug` | Development, testing, sanitizers, coverage |
| Release | `Release` | Speed optimization (`-O2` / `/O2`) |
| RelWithDebInfo | `RelWithDebInfo` | Speed optimization + debug info |
| MinSizeRel | `MinSizeRel` | Size optimization (`-Os` / `/O1`) |

On Unix, additionally strip debug symbols from the final binary after build:

```bash
strip out/{target}
```

Where `{target}` is the executable or shared library (`{name}` / `lib{name}.so` / `lib{name}.dylib`). Do not strip static libraries (`.a`) -- `strip` removes symbols needed by the linker.

## Platform Strategy

| Platform | Generator | Type | Reason |
|----------|-----------|------|--------|
| Windows | Ninja (via MSVC Developer Shell) | Single-config | Produces `compile_commands.json` for clangd |
| Linux/macOS | Ninja | Single-config | Produces `compile_commands.json` for clangd |

All platforms use Ninja as the generator. This ensures `CMAKE_EXPORT_COMPILE_COMMANDS` produces `compile_commands.json` in the build directory. Visual Studio generators do not produce this file.

On Windows, Ninja requires the MSVC environment. When running commands manually, the user must have the MSVC environment active (e.g. via `vcvarsall.bat x64`, "Developer PowerShell for VS", or `Launch-VsDevShell.ps1`).

## compile_commands.json

After every successful configure or build, copy `compile_commands.json` from the build directory to the project root so clangd can find it:

| Platform | Command |
|----------|---------|
| Unix | `cp -f out/compile_commands.json compile_commands.json 2>/dev/null \|\| true` |
| Windows | `copy /Y out\compile_commands.json compile_commands.json 2>nul` |

This file should be in `.gitignore`.

## Commands

### Configure

**MANDATORY: Always delete `out/` before configuring.** CMake caches option values; if a previous configure had `TLS=ON` and you now configure with `TLS=OFF`, stale object files from the previous build may remain and link into the final binary. Always start fresh:

```bash
rm -rf out   # Unix
```
```bat
rmdir /s /q out   &:: Windows
```

All platforms use single-config Ninja. Build type is set at configure time via `-DCMAKE_BUILD_TYPE`.

#### Unix (Linux / macOS)

```bash
cmake -B out -G Ninja \
  -DCMAKE_BUILD_TYPE={build_type} \
  [-D{NAME}_ENABLE_TESTING=ON] \
  [-D{NAME}_ENABLE_COVERAGE=OFF] \
  [-D{NAME}_ENABLE_{FEATURE}=ON]
```

#### Windows (MSVC + Ninja)

Requires MSVC environment active (e.g. `vcvarsall.bat x64` or Developer PowerShell for VS).

```bat
cmake -B out -G Ninja ^
  -DCMAKE_BUILD_TYPE={build_type} ^
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
  [-D{NAME}_ENABLE_TESTING=ON] ^
  [-D{NAME}_ENABLE_COVERAGE=OFF] ^
  [-D{NAME}_ENABLE_{FEATURE}=ON]
```

Note: On Unix, `CMAKE_EXPORT_COMPILE_COMMANDS` is already set to `ON` in the project's `CMakeLists.txt`. On Windows with Ninja, pass it explicitly as a safety measure since some environments may not honor the CMakeLists.txt setting.

### Build

```bash
cmake --build out -j {ncpu}
```

Detect CPU count:
- Linux: `nproc`
- macOS: `sysctl -n hw.ncpu`
- Windows: `%NUMBER_OF_PROCESSORS%` or default `8`
- Fallback: `4`

After build, copy compile_commands.json to project root (see above).

### Test

```bash
ctest --test-dir out --output-on-failure
```

Run a single test module:

```bash
ctest --test-dir out -R {module} --output-on-failure
```

### Sanitizers

Enable one sanitizer at a time. ASAN and TSAN cannot coexist.

```bash
cmake -B out -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -D{NAME}_ENABLE_TESTING=ON \
  -D{NAME}_ENABLE_ASAN=ON

cmake --build out -j {ncpu}
ctest --test-dir out --output-on-failure
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
cmake -B out -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -D{NAME}_ENABLE_TESTING=ON \
  -D{NAME}_ENABLE_COVERAGE=ON

cmake --build out -j {ncpu}
cmake --build out --target coverage
```

Requires `lcov` and `genhtml`. HTML report at `out/coverage/html/index.html`.

#### Windows

Windows coverage uses OpenCppCoverage, an external runtime instrumentation tool — no compiler flags needed. The `coverage` CMake target is typically defined in `tests/CMakeLists.txt` (not the root), guarded by `if({NAME}_ENABLE_COVERAGE AND WIN32)` with `find_program(OPENCPPCOVERAGE_BIN OpenCppCoverage)`.

Because OpenCppCoverage instruments at runtime (not compile time), the library itself does not need recompilation. Only the `{NAME}_ENABLE_COVERAGE=ON` flag is needed so CMake creates the `coverage` target.

```bat
cmake -B out -G Ninja ^
  -DCMAKE_BUILD_TYPE=Debug ^
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
  -D{NAME}_ENABLE_TESTING=ON ^
  -D{NAME}_ENABLE_COVERAGE=ON

cmake --build out -j 8
cmake --build out --target coverage
```

HTML report at `out/coverage/index.html`.

If `cmake --build out --target coverage` fails with "no rule to make target 'coverage'", OpenCppCoverage is not installed or not on PATH. Install it from https://github.com/OpenCppCoverage/OpenCppCoverage/releases and ensure `OpenCppCoverage.exe` is on PATH, then reconfigure.

Important: Do NOT assume Windows lacks coverage support just because the root `CMakeLists.txt` only has `if({NAME}_ENABLE_COVERAGE AND UNIX)`. Always check `tests/CMakeLists.txt` (or subdirectory CMakeLists) for the Windows coverage target before concluding coverage is unavailable.

## Reconfigure vs Rebuild

| Scenario | Action |
|----------|--------|
| Changed source files only | Build only (cmake --build out) |
| Any other change (options, build type, CMakeLists.txt, sanitizer) | Delete `out/` and reconfigure from scratch |

**Rule: Every configure = delete `out/` first.** This prevents stale cache from producing incorrect builds (e.g. TLS code linking in after disabling TLS option).

```bash
rm -rf out
# then configure + build
```

