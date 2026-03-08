# Deployment

> This file defines deployment procedures for AI agents packaging and releasing C projects built with this skill.

## Artifact Types

| Project Type | Artifact | Contents |
|-------------|----------|----------|
| Library (static) | `lib/{project}.a` (Unix) / `{project}.lib` (Windows) | Static archive |
| Library (shared) | `lib/{project}.so` (Linux) / `{project}.dylib` (macOS) / `{project}.dll` (Windows) | Dynamic library + import lib on Windows |
| Application | `bin/{project}` (Unix) / `{project}.exe` (Windows) | Executable binary |
| Headers (library only) | `include/{project}.h` + `include/{project}/` | Public API headers |

## Install

CMake `install()` targets are defined in the root `CMakeLists.txt`. Run:

```bash
# Single-config (Linux/macOS)
cmake -B out -DCMAKE_BUILD_TYPE=Release
cmake --build out
cmake --install out

# Multi-config (Windows)
cmake -B out
cmake --build out --config Release
cmake --install out --config Release
```

Default install prefix is `out/install/` (set in CMakeLists.txt). Override with:

```bash
cmake -B out -DCMAKE_INSTALL_PREFIX=/usr/local
```

### Install Layout

```
{install_prefix}/
├── bin/              # Executables
├── lib/              # Static/shared libraries
└── include/          # Headers (library projects only)
    ├── {project}.h
    └── {project}/
```

## Release Checklist

Before tagging a release:

| Step | Command / Action | Pass Criteria |
|------|-----------------|---------------|
| 1. All tests pass | `ctest --test-dir out --output-on-failure` | Exit code 0, no failures |
| 2. ASAN clean | `cmake -B out -D{PROJECT}_ENABLE_ASAN=ON && cmake --build out && ctest --test-dir out --output-on-failure` | No ASAN errors |
| 3. UBSAN clean | `cmake -B out -D{PROJECT}_ENABLE_UBSAN=ON && cmake --build out && ctest --test-dir out --output-on-failure` | No UBSAN errors |
| 4. No compiler warnings | Build with `-Werror` (GCC/Clang) or `/WX` (MSVC) | Zero warnings |
| 5. No unreplaced placeholders | `grep -rn '{project}\|{PROJECT}\|{YEAR}\|{AUTHOR}\|{EMAIL}\|{DESCRIPTION}' src/ include/ tests/` | No matches |
| 6. Version updated | Check version string in source or CMakeLists.txt | Matches intended release |
| 7. CHANGELOG updated | Review `CHANGELOG.md` (if present) | Entry exists for this version |

## Versioning

Use [Semantic Versioning](https://semver.org/):

| Change | Version Bump | Example |
|--------|-------------|---------|
| Breaking API change (removed function, changed signature) | Major | `1.2.3` → `2.0.0` |
| New public function or feature, backward-compatible | Minor | `1.2.3` → `1.3.0` |
| Bug fix, no API change | Patch | `1.2.3` → `1.2.4` |

### Version Location

Store the version in `CMakeLists.txt` using the `project()` command:

```cmake
project({PROJECT} VERSION 1.0.0 LANGUAGES C)
```

Access in code via CMake-generated defines or a version header.

## Packaging

### Source Archive

```bash
git archive --format=tar.gz --prefix={project}-{version}/ -o {project}-{version}.tar.gz HEAD
```

### Binary Package (CPack)

If CPack is configured in CMakeLists.txt:

```bash
# Single-config
cmake -B out -DCMAKE_BUILD_TYPE=Release
cmake --build out
cpack --config out/CPackConfig.cmake

# Multi-config
cmake -B out
cmake --build out --config Release
cpack --config out/CPackConfig.cmake -C Release
```

## CI Integration

### Minimum CI Matrix

| Platform | Compiler | Build Type |
|----------|----------|------------|
| Ubuntu (latest LTS) | GCC | Debug, Release |
| Ubuntu (latest LTS) | Clang | Debug |
| macOS (latest) | Apple Clang | Debug, Release |
| Windows (latest) | MSVC | Debug, Release |

### CI Steps

```yaml
# Pseudocode — adapt to your CI system
steps:
  - configure: cmake -B out -DCMAKE_BUILD_TYPE=$BUILD_TYPE -D{PROJECT}_ENABLE_TESTING=ON
  - build: cmake --build out
  - test: ctest --test-dir out --output-on-failure
  - sanitizer: cmake -B out-asan -D{PROJECT}_ENABLE_ASAN=ON && cmake --build out-asan && ctest --test-dir out-asan --output-on-failure
```

### CI Rules

| Rule | Detail |
|------|--------|
| All matrix entries must pass | Do not release if any platform/compiler combination fails |
| ASAN runs on every PR | Memory errors must be caught before merge |
| Release builds use `-DCMAKE_BUILD_TYPE=Release` | Do not ship Debug builds |
| Artifacts are built from tagged commits only | No artifacts from untagged or dirty trees |

## Prohibited Patterns

| Pattern | Why | Fix |
|---------|-----|-----|
| Shipping with `{PROJECT}_ENABLE_ASAN=ON` | Sanitizer runtime adds overhead and changes behavior | Build release without sanitizers |
| Hardcoded install paths in CMakeLists.txt | Breaks portability across systems | Use `CMAKE_INSTALL_PREFIX` and `GNUInstallDirs` |
| Missing `install()` for public headers | Users get the library but not the API | Add `install(DIRECTORY include/ ...)` |
| Version string only in source code | CMake and CPack cannot read it | Use `project(... VERSION x.y.z)` |
