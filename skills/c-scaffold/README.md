# C Scaffold

Scaffold cross-platform C projects from templates. Supports library and application project types.

## Contents

```
c-scaffold/
├── SKILL.md              # Skill definition (metadata + routing table)
├── README.md             # This file (for humans)
├── references/
│   └── setup.md          # Project setup checklist
└── assets/
    └── templates/        # Project scaffolding
        ├── common/       # Shared files (.clang-format, .gitignore, cmake/, tests/, docs/)
        ├── library/      # Library-specific (CMakeLists.txt, include/, examples/)
        └── application/  # Application-specific (CMakeLists.txt)
```

## Dependencies

| Tool | Purpose | Install |
|------|---------|---------|
| CMake >= 3.16 | Build system | [cmake.org](https://cmake.org/download/) |
| C compiler | MSVC / GCC / Clang | VS2022 (Windows), `apt install build-essential` (Linux), Xcode CLT (macOS) |
| clang-format | Code formatting | Included with LLVM: [llvm.org](https://releases.llvm.org/) |
