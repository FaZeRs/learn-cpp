# Reference Counting in C++

This example demonstrates a custom implementation of reference counting in C++, similar to `std::shared_ptr`. Reference counting is a memory management technique where objects maintain a count of how many references point to them. When the count reaches zero, the object is automatically deleted.

## Features

- Thread-safe reference counting using `std::atomic`
- Move and copy semantics
- Debug information including creation location
- RAII-compliant memory management
- Comparison operators
- Null state handling

## Implementation Details

The implementation consists of two main components:

1. **RefCounted<T> Template Class**: A smart pointer that manages the lifetime of objects through reference counting.
2. **ControlBlock**: An internal structure that holds the actual data, reference count, and debugging information.

### Key Components

#### Control Block
The control block maintains:
- Pointer to the managed object
- Atomic reference count
- Source location information for debugging
- Automatic cleanup when reference count reaches zero

#### RefCounted<T> Operations
- Construction with perfect forwarding
- Copy/Move semantics
- Comparison operators
- Pointer-like operations (`operator*`, `operator->`)
- Debug information access
- Reference count querying

## Usage Example

```cpp
// Create a reference-counted resource
RefCounted<Resource> res1{"MyResource"};

// Create additional references
auto res2 = res1;  // Reference count = 2
auto res3 = res2;  // Reference count = 3

// Access the managed object
std::println("Resource name: {}", res3->getName());

// Check reference count
std::println("Reference count: {}", res1.use_count());

// Move ownership
auto res4 = std::move(res1);  // res1 becomes null

// Explicit cleanup
res4.reset();  // Releases the resource if this is the last reference
```

## Safety Features

1. **Thread Safety**: Reference counting operations are atomic
2. **RAII Compliance**: Resources are automatically cleaned up
3. **Null Safety**: Checks for null pointer dereference
4. **Debug Support**: Creation location tracking and formatted debug output

## Implementation Notes

The implementation uses modern C++ features including:
- Concepts for type constraints
- Source location for debugging
- Atomic operations for thread safety
- Perfect forwarding for efficient construction
- Three-way comparison operator (<=>)

## Best Practices

1. Use `reset()` to explicitly release resources when needed
2. Check `unique()` to determine exclusive ownership
3. Use `debug_info()` for troubleshooting
4. Prefer move semantics when transferring ownership
5. Avoid circular references as they can lead to memory leaks

## Limitations

1. Does not handle circular references automatically
2. Additional memory overhead for the control block
3. Slight performance overhead from atomic operations
4. Not suitable for arrays (unlike `std::shared_ptr`)

## Requirements

- C++20 or later
- Compiler with concepts support
- Compiler with three-way comparison operator support
