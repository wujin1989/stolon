# Build Workflow

## Overview

This skill drives the build/test/coverage cycle for C11 CMake projects that follow the `c-project-init` conventions. All commands assume the project root contains a `CMakeLists.txt` with a valid `project()` call.

Placeholders:
- `{name}` -- lowercase project name from `CMakeLists.txt`
- `{NAME}` -- uppercase form

## Inputs

Before running any command, determine:

| Input | How to obtain | Default |
|-------|---------------|---------|
| Project name | Read `project(...)` from root `CMakeLists.txt` | (required) |
| Build type | Ask user: Debug or Release | (required, must ask) |
| Feature flags | Ask user which optional features to enable | None |
| Sanitizer | Ask user if needed | None |
| Coverage | Ask user if needed | OFF |

Feature flags are project-specific `option()` entries in `CMakeLists.txt` (e.g. `{NAME}_ENABLE_TLS`). Scan the root `CMakeLists.txt` for all `option(...)` lines to discover available flags. Only enable flags the user explicitly requests.

## Build Type

Always ask the user whether to build Debug or Release before configuring.

| Build type | `CMAKE_BUILD_TYPE` | Use case |
|------------|--------------------|----------|
| Debug | `Debug` | Development, testing, sanitizers, coverage |
| Release | `MinSizeRel` | Production, smallest binary size |

When the user says "Release", use `MinSizeRel` (not `Release`). This enables `-Os` (GCC/Clang) or `/O1` (MSVC), producing the smallest library or executable. `Release` uses `-O3` which optimizes for speed and often increases binary size -- not what we want here.

On Unix, additionally strip debug symbols from the final binary after build:

```bash
strip out/{target}
```

Where `{target}` is the executable or shared library (`{name}` / `lib{name}.so` / `lib{name}.dylib`). Do not strip static libraries (`.a`) -- `strip` removes symbols needed by the linker.

## Platform Strategy

| Platform | Generator | Type | Reason |
|----------|-----------|------|--------|
| Windows | Ninja (via MSVC vcvarsall) | Single-config | Produces `compile_commands.json` for clangd |
| Linux/macOS | Ninja | Single-config | Produces `compile_commands.json` for clangd |

All platforms use Ninja as the generator. This ensures `CMAKE_EXPORT_COMPILE_COMMANDS` produces `compile_commands.json` in the build directory. Visual Studio generators do not produce this file.

On Windows, Ninja requires the MSVC environment. The `build.bat` script calls `vcvarsall.bat x64` before cmake. When running commands manually, the user must have the MSVC environment active (e.g. via "Developer Command Prompt" or by sourcing `vcvarsall.bat`).

## compile_commands.json

After every successful configure or build, copy `compile_commands.json` from the build directory to the project root so clangd can find it:

| Platform | Command |
|----------|---------|
| Unix | `cp -f out/compile_commands.json compile_commands.json 2>/dev/null \|\| true` |
| Windows | `copy /Y out\compile_commands.json compile_commands.json 2>nul` |

This file should be in `.gitignore` (the `c-project-init` skill already adds it).

## Commands

### Configure

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

Requires MSVC environment active (vcvarsall.bat x64).

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

```bat
cmake -B out -G Ninja ^
  -DCMAKE_BUILD_TYPE=Debug ^
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
  -D{NAME}_ENABLE_TESTING=ON ^
  -D{NAME}_ENABLE_COVERAGE=ON

cmake --build out -j 8
cmake --build out --target coverage
```

Requires OpenCppCoverage. HTML report at `out/coverage/index.html`.

## Reconfigure vs Rebuild

| Scenario | Action |
|----------|--------|
| Changed CMakeLists.txt or options | Re-run configure (cmake -B out ...) then build |
| Changed source files only | Build only (cmake --build out) |
| Switching build type or sanitizer | Delete `out/` and reconfigure from scratch |
| Added new source files to SRCS | Re-run configure then build |

When switching between Debug/Release or toggling sanitizers, always start fresh:

```bash
rm -rf out
# then configure + build
```

## build.sh / build.bat

Projects generated by `c-project-init` include convenience scripts that wrap the full cycle (configure + build + copy compile_commands.json + test + coverage). These scripts:

1. Remove `out/` for a clean build
2. Configure with Ninja, Debug, testing ON
3. Build
4. Copy `compile_commands.json` to project root
5. Run tests
6. Generate coverage report
7. Open coverage HTML if available

The scripts are a quick-start convenience. For incremental builds or custom configurations, use the individual cmake commands above.
