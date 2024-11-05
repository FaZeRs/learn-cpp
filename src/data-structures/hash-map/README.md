# Hash Map

A Hash Map (also known as Hash Table) is a data structure that implements an associative array abstract data type, a structure that can map keys to values. It uses a hash function to compute an index into an array of buckets or slots, from which the desired value can be found.

## Time Complexity

| Operation | Average | Worst Case |
|-----------|---------|------------|
| Access    | N/A     | N/A        |
| Search    | O(1)    | O(n)       |
| Insertion | O(1)    | O(n)       |
| Deletion  | O(1)    | O(n)       |

## Space Complexity

O(n), where n is the number of key-value pairs stored in the hash map.

## Key Features

- **Fast Access**: Provides constant-time O(1) average case performance for insertions, deletions, and lookups
- **Key-Value Storage**: Stores data in key-value pairs
- **Dynamic Size**: Can grow and shrink as needed
- **Collision Handling**: Uses techniques like chaining or open addressing to handle hash collisions

## Common Use Cases

1. Caching
2. Database Indexing
3. Symbol Tables in Compilers
4. Counting Frequencies
5. Implementing Sets
6. Implementing Associative Arrays

## Implementation Details

The hash map implementation typically includes:

- A hash function to convert keys into array indices
- An array to store the key-value pairs
- Collision resolution mechanism (e.g., chaining with linked lists)
- Load factor management for resizing
- Methods for basic operations (put, get, remove)

## Basic Operations

```c++
// Creating a hash map
std::unordered_map<std::string, std::string> map;

// Adding key-value pairs
map["key1"] = "value1";
map["key2"] = "value2";

// Getting values
std::string value = map["key1"]; // Returns "value1"

// Removing entries
map.erase("key2");

// Checking if key exists
bool has_key = map.find("key1") != map.end(); // Returns true
```

## Common Issues and Solutions

1. **Hash Collisions**
   - Solution: Use chaining or open addressing
   - Impact: Can degrade performance in worst case

2. **Load Factor**
   - Solution: Implement dynamic resizing
   - Impact: Affects space-time trade-off

3. **Poor Hash Function**
   - Solution: Use well-distributed hash functions
   - Impact: Affects overall performance

## Related Data Structures

- HashSet
- LinkedHashMap
- TreeMap
- Dictionary

## References

- [Hash Table on Wikipedia](https://en.wikipedia.org/wiki/Hash_table)
- [Hash Table Visualization](https://www.cs.usfca.edu/~galles/visualization/OpenHash.html)
