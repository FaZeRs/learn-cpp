# RAII (Resource Acquisition Is Initialization)

## Overview

RAII is a C++ programming idiom that ties resource management to object lifetime. Resources (like memory, file handles, or network connections) are acquired during object construction and automatically released during object destruction.

## Key Principles

- Resources are acquired in the constructor
- Resources are released in the destructor
- Objects manage one resource only (Single Responsibility Principle)
- Resources are never exposed directly

## Benefits

1. Automatic resource management
2. Exception-safe code
3. Clear ownership semantics
4. Prevention of resource leaks

## Common Use Cases

```cpp
// Example 1: Smart Pointers
std::unique_ptr<MyClass> ptr(new MyClass()); // Memory automatically freed

// Example 2: File Handling
class FileHandler {
    std::ifstream file;
public:
    FileHandler(const std::string& filename) : file(filename) {}
    // File automatically closed when object is destroyed
    ~FileHandler() {
        if (file.is_open()) {
            file.close();
        }
    }
};

// Example 3: Lock Guards
{
    std::lock_guard<std::mutex> lock(mutex); // Automatically unlocks
    // Protected code here
}
```

## Best Practices

1. Always use RAII containers and wrappers
   - `std::unique_ptr` and `std::shared_ptr` for dynamic memory
   - `std::lock_guard` for mutexes
   - `std::fstream` for file operations

2. Implement the Rule of Five when managing resources:
   - Destructor
   - Copy constructor
   - Copy assignment operator
   - Move constructor
   - Move assignment operator

3. Never use raw pointers for ownership

## Common RAII Classes in STL

- Smart Pointers: `unique_ptr`, `shared_ptr`, `weak_ptr`
- Containers: `vector`, `string`, `array`
- Stream Classes: `fstream`, `stringstream`
- Thread Synchronization: `lock_guard`, `unique_lock`

## Anti-patterns to Avoid

- Manual resource management with `new`/`delete`
- Explicit calls to cleanup functions
- Mixing RAII and manual resource management
- Multiple responsibility in resource-managing classes

## References

- [C++ Core Guidelines: RAII](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rr-raii)
- [CPPReference: RAII](https://en.cppreference.com/w/cpp/language/raii)
