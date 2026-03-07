# Project Structure

> Recommended trigger: manual inclusion.

```
include/
  {project}.h                      # Umbrella header — includes all module headers
  {project}/{project}-<module>.h   # Public API (one header per module)
src/
  {project}-<module>.c             # Implementation (one source per module)
  platform/platform.h              # Platform-specific abstractions
tests/
  assert.h                         # Custom ASSERT macro
  test-<module>.c                  # Unit tests (one file per module)
  CMakeLists.txt                   # Test registration via {project}_add_test()
examples/
  CMakeLists.txt                   # Example programs
cmake/
  {project}-utils.cmake            # Helpers: sanitizer setup, test registration
docs/
  build.md                         # Build instructions
```

## Adding a New Module

1. Create `include/{project}/{project}-<module>.h` — use `_Pragma("once")`, include `"{project}.h"`, declare public API with `extern`
2. Create `src/{project}-<module>.c` — license header, `#include "{project}.h"`, implement functions
3. Add `src/{project}-<module>.c` to `SRCS` in root `CMakeLists.txt`
4. Add `#include "{project}/{project}-<module>.h"` to `include/{project}.h`
5. Create `tests/test-<module>.c` — test functions + `main()` calling them
6. Add `{project}_add_test(<module>)` to `tests/CMakeLists.txt`

## Intrusive Data Structures

Heap, rbtree, list, stack, queue, and similar modules use intrusive nodes. Users embed a `{project}_<module>_node_t` in their struct and recover the container with:

```c
{project}_<module>_entry(node_ptr, container_type, member_name)
```
