# LRU Cache Implementation

A modern C++20 implementation of a Least Recently Used (LRU) cache with template support for any hashable key type and any value type.

## Features

- Generic implementation supporting any hashable key type and any value type
- Constant time O(1) operations for all primary functions
- Modern C++20 features including concepts and constexpr support
- Thread-unsafe but memory safe implementation
- STL-style iterator interface
- Move semantics support

## API Reference

### Construction

- `LRUCache(size_t capacity)` - Creates a new cache with specified maximum capacity

### Core Operations

- `std::optional<Value> get(const Key& key)` - Retrieves value and marks as most recently used
- `void put(const Key& key, const Value& value)` - Inserts or updates value
- `void emplace(const Key& key, Args&&... args)` - Constructs value in-place
- `bool contains(const Key& key)` - Checks if key exists in cache

### Capacity Operations

- `size_t size()` - Current number of elements
- `size_t max_size()` - Maximum capacity
- `bool empty()` - Checks if cache is empty
- `bool full()` - Checks if cache is at capacity
- `void clear()` - Removes all elements

### Iteration

- `begin()/end()` - Iterator access (from most to least recently used)
- `cbegin()/cend()` - Const iterator access

## Implementation Details

The cache is implemented using two primary data structures:
- `std::list` to maintain the order of elements (most recent to least recent)
- `std::unordered_map` for O(1) key lookups

When the cache is full and a new item is added, the least recently used item is automatically evicted.

## Requirements

- C++20 compatible compiler
- Standard Template Library (STL)

## Template Parameters

- `Key`: Must satisfy the `HashableKey` concept (regular type with std::hash support)
- `Value`: Any type that satisfies regular C++ type requirements

## Performance

All primary operations (get, put, contains) have O(1) time complexity:
- Get: O(1)
- Put: O(1)
- Contains: O(1)
- Iteration: O(n)

Space complexity is O(capacity) where capacity is the maximum number of elements.
