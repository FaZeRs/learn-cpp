# Circular Buffer

A Circular Buffer (also known as a Ring Buffer) is a fixed-size, FIFO (First-In-First-Out) data structure that uses a single, fixed-size buffer as if it were connected end-to-end. It is particularly useful for buffering data streams or implementing a rolling window of fixed size.

## Features

- Fixed-size circular buffer implementation in modern C++
- Template-based design supporting any copyable and default-initializable type
- STL-compatible container with range-based for loop support
- Full C++23 ranges and views integration, including enumerate view
- Thread-safe operations using mutex protection
- Efficient view-based iteration with std::span
- Compile-time size checking
- Exception-free operations (all operations are `noexcept`)

## Usage

### Basic Operations

```cpp
// Create a buffer with initial values
CircularBuffer buffer{1, 2, 3, 4, 5};

// Add elements
buffer.push(6);
buffer.emplace(7);  // Construct element in-place

// Remove elements
auto value = buffer.pop();  // Returns std::optional<T>

// Check state
bool is_empty = buffer.empty();
bool is_full = buffer.full();
size_t current_size = buffer.size();
size_t max_size = buffer.capacity();
```

### Range-based Operations

```cpp
// Iterate over elements
for (const auto& item : buffer) {
    // Process item
}

// Use with ranges and views
auto even_numbers = buffer | std::views::filter([](int n) { return n % 2 == 0; });
auto reversed = buffer | std::views::reverse;
```

## Implementation Details

The circular buffer is implemented using:
- A fixed-size array for storage
- Read and write positions that wrap around using modulo arithmetic
- A count to track the number of elements
- A mutex for thread-safety

Key features of the implementation:

1. **Thread Safety**: All operations are protected by a mutex
2. **Memory Efficiency**: Uses a fixed-size array without dynamic allocation
3. **View-based Access**: Uses std::span for efficient iteration
4. **STL Compatibility**: Implements necessary interfaces for STL algorithms and ranges
5. **Modern C++23 Features**: Full support for ranges, views, and print facilities

## Requirements

- C++23 or later
- Compiler with concepts support
- CMake 3.12 or later for building

## Performance Characteristics

- Push/Emplace: O(1)
- Pop: O(1)
- Size/Empty/Full checks: O(1)
- Contains: O(n)
- Iterator operations: O(1)

## Example

```cpp
CircularBuffer<int, 5> buffer{1, 2, 3};

// Add elements
buffer.push(4);
buffer.push(5);
buffer.push(6);  // Overwrites 1 since buffer is full

// Use with ranges
auto sum = std::ranges::fold_left(buffer, 0, std::plus{});
auto [min, max] = std::ranges::minmax(buffer);

// Check for elements
bool has_three = buffer.contains(3);
```
