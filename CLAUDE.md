# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a C++ learning project demonstrating implementations of data structures, algorithms, design patterns, memory management concepts, and modern C++ features. Each subdirectory typically contains a `main.cpp` with a standalone example/implementation.

## Build System

**CMake Version**: 3.27 or higher
**C++ Standard**: C++23
**Build Generator**: Ninja (Linux/macOS) or Visual Studio 17 2022 (Windows)

### Build Commands

**Initial configuration:**
```bash
cmake -E make_directory build
cd build
cmake -G "Ninja" .. -DCMAKE_BUILD_TYPE=Release  # or Debug
```

**Build:**
```bash
cmake --build . --config Release  # or Debug
```

**Parallel build:**
```bash
cmake --build . --parallel <N> --config Release
```

### Build Options

Configure via CMake options (use `-D<OPTION>=ON/OFF`):

- `WARNINGS_AS_ERRORS` (default: ON) - Treat compiler warnings as errors
- `ENABLE_CLANG_TIDY` (default: ON) - Run clang-tidy static analysis
- `ENABLE_CPPCHECK` (default: ON) - Run cppcheck analysis
- `ENABLE_INCLUDE_WHAT_YOU_USE` (default: ON) - Enable include-what-you-use checks
- `ENABLE_CACHE` (default: OFF) - Enable ccache for faster rebuilds
- `ENABLE_SANITIZER_ADDRESS` (default: OFF) - Enable address sanitizer
- `ENABLE_SANITIZER_LEAK` (default: OFF) - Enable leak sanitizer
- `ENABLE_SANITIZER_UNDEFINED` (default: OFF) - Enable undefined behavior sanitizer
- `ENABLE_SANITIZER_THREAD` (default: OFF) - Enable thread sanitizer
- `ENABLE_SANITIZER_MEMORY` (default: OFF) - Enable memory sanitizer

Example with sanitizers:
```bash
cmake -G "Ninja" .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZER_ADDRESS=ON -DENABLE_SANITIZER_UNDEFINED=ON
```

### Running Executables

Executables are built to `build/bin/`. Each module/example has its own executable:
```bash
./build/bin/<category>-<example>
# Examples:
./build/bin/data-structures-linked-list
./build/bin/patterns-singleton
./build/bin/concurrency-thread-pool
```

## Project Architecture

### Source Organization

The `src/` directory is organized into topical subdirectories, where each subdirectory contains multiple self-contained examples:

- **concurrency/** - Threading primitives, synchronization (atomic, mutex, threads, thread-pool, omp, intel-tbb, parallel)
- **data-structures/** - Common data structures (array, linked-list, binary-tree, hash-map, heap, queue, stack, vector, lru-cache, circular-buffer)
- **memory/** - Memory management patterns (raii, pools, reference-counting, move-semantics)
- **patterns/** - Design patterns organized by category:
  - **behavioral/** - Chain of responsibility, command, iterator, mediator, memento, observer, state, strategy, template-method, visitor
  - **creational/** - Abstract factory, builder, factory-method, prototype, singleton
  - **structural/** - Adapter, bridge, composite, decorator, facade, flyweight, proxy
- **templates/** - Template metaprogramming (vardiac, tmp, sfinae)
- **sorting/** - Sorting algorithms (bubble-sort, quick-sort)
- **search/** - Search algorithms (binary-search, linear-search, ternary-search)
- **simd/** - SIMD examples
- **filesystem/** - Filesystem operations (watcher)
- **real-world/** - Practical applications (command-line, file-manager, logger, signals, ui, undo-framework)
- **modules/** - C++20 modules (only compiled with Clang due to compiler support)

### CMake Structure

The project uses a hierarchical CMake structure:
1. Root `CMakeLists.txt` sets up global options, warnings, sanitizers, and static analyzers
2. `src/CMakeLists.txt` includes subdirectories
3. Each category subdirectory has its own `CMakeLists.txt`
4. Individual examples may have their own `CMakeLists.txt` for specific configurations

### Custom CMake Modules

Located in `cmake/` directory and sourced from `gh:fazers/cmake#1.2.0`:
- `compiler-warnings` - Comprehensive warning configuration
- `sanitizers` - Memory/thread sanitizer setup
- `static-analyzers` - clang-tidy, cppcheck, include-what-you-use integration
- `cache` - ccache configuration
- `CPM.cmake` - CMake Package Manager for dependencies

## Compiler Support

**Tested compilers:**
- GCC 14
- LLVM/Clang 20

**Platform support:**
- Linux (Ubuntu 24.04)

**Note**: The `modules/` directory only builds with Clang due to C++20 modules support requirements.

## Dependencies

Managed via system packages (Linux):
- **Intel TBB** (Threading Building Blocks) - Required for `src/concurrency/intel-tbb/`
- **X11** (Linux only) - Required for certain GUI examples

## Code Quality Tools

**Static analyzers** (enabled by default):
- clang-tidy
- cppcheck
- include-what-you-use

**Code formatting:**
- `.clang-format` - Clang format configuration
- `.clang-tidy` - Clang-tidy checks configuration
- `.cmake-format.yaml` - CMake formatting

## CI/CD

The project uses GitHub Actions CI (`.github/workflows/ci.yml`) with a build matrix testing:
- Single OS version (Ubuntu 24.04)
- Multiple compilers (GCC-14, LLVM-20)
- Both Release and Debug configurations
- Static analysis on all builds

## Adding New Examples

When adding a new example:
1. Create a subdirectory under the appropriate category in `src/`
2. Add a `main.cpp` with the implementation
3. Create a `CMakeLists.txt` in the subdirectory or add to the parent's CMakeLists.txt
4. Follow the existing pattern: each example is a standalone executable
5. Optional: Add a `README.md` explaining the concept, complexity, and use cases

## Release Configuration

Release builds use `-march=native` for GCC/Clang to optimize for the build machine's architecture. Debug builds use `-Og -ggdb` for better debugging experience.
