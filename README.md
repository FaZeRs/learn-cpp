# C++ Learning Project

A comprehensive C++ project showcasing implementations of common data structures, algorithms, design patterns, and modern C++ features. This project serves as both a learning resource and a reference implementation.

## 🎯 Project Overview

This project includes implementations and examples of:

- Data Structures (Binary Trees, Hash Maps, Linked Lists, etc.)
- Algorithms (Sorting, Searching)
- Design Patterns (Creational, Structural, Behavioral)
- Modern C++ Features (Templates, RAII)
- Memory Management Concepts

## 🚀 Getting Started

### Prerequisites

- CMake 3.27 or higher
- Modern C++ Compiler (GCC-14, LLVM-20)
- Ninja or Visual Studio 2022 (for Windows)

### Building the Project

1. Clone the repository
```bash
git clone https://github.com/FaZeRs/learn-cpp.git
cd learn-cpp
```

2. Create a build directory
```bash
mkdir build && cd build
```

3. Configure and build
```bash
cmake -G "Ninja" ..
cmake --build .
```

## 🏗️ Project Structure

- `src/` - Main source code directory
  - `data-structures/` - Implementation of common data structures
  - `algorithms/` - Various algorithm implementations
  - `patterns/` - Design pattern examples
  - `templates/` - Template metaprogramming examples
  - `memory/` - Memory management concepts

## 🛠️ Development Environment

This project includes a complete development environment setup using:

- DevContainer configuration for VS Code
- Pre-configured development tools and extensions
- Automated CI/CD pipelines
- Code quality tools (clang-tidy, cppcheck)

### VS Code Extensions

The project comes with recommended extensions for C++ development:

- C/C++ tools
- CMake tools
- Doxygen documentation generator
- Git integration
- Code formatting and analysis tools

## 🧪 Quality Assurance

The project employs several tools to maintain code quality:

- Static Analysis (clang-tidy, cppcheck)
- CodeQL Analysis
- Automated CI/CD pipelines
- Comprehensive build matrix testing

## 📚 Learning Resources

Each implementation includes detailed documentation explaining:

- Theoretical concepts
- Time and space complexity
- Common use cases
- Code examples
- References for further reading

## 🤝 Contributing

Contributions are welcome! Please feel free to submit pull requests. Make sure to:

1. Follow the existing code style
2. Add appropriate documentation
3. Update relevant README files

## 📄 License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## 🔗 References

- [C++ Reference](https://en.cppreference.com/)
- [Design Patterns](https://refactoring.guru/design-patterns)
- [Data Structures and Algorithms](https://www.geeksforgeeks.org/)
