---
sidebar-position: 5
---

# CMake

### Get `clangd` Working

Ensure we do `set(CMAKE_EXPORT_COMPILE_COMMANDS ON)`
- This exports `compile_commands.json`, a compilation DB
- Symbols, include paths can be known to LSP using this

### Adding a Dependency

e.g. `fmt`:

```cmake title="CMakeLists.txt"
# enable fetchcontent module
include(FetchContent)

# declare dependency; specify git repo + version
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.2.1
)

# download, build, and add to project
FetchContent_MakeAvailable(fmt)

# link it to exe!
target_link_libraries(MyProject PRIVATE fmt::fmt)
```

- `PRIVATE` in `target_link` links the dep. to our project, but doesn't expose it to anything that includes our project
- We don't always target link like `fmt::fmt`; read the docs to know how!
