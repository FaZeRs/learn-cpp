# Stack

A Stack is a linear data structure that follows the Last-In-First-Out (LIFO) principle. Think of it like a stack of plates - you can only add or remove plates from the top.

## Basic Operations

- **Push**: Add an element to the top of the stack
- **Pop**: Remove and return the top element from the stack
- **Peek/Top**: View the top element without removing it
- **isEmpty**: Check if the stack is empty
- **Size**: Get the number of elements in the stack

## Time Complexity

| Operation | Time Complexity |
|-----------|----------------|
| Push      | O(1)           |
| Pop       | O(1)           |
| Peek      | O(1)           |
| isEmpty   | O(1)           |
| Size      | O(1)           |

## Common Use Cases

1. Function call management (Call Stack)
2. Undo/Redo operations in editors
3. Expression evaluation and syntax parsing
4. Browser history (Back button functionality)
5. Backtracking algorithms

## Implementation Example

```c++
#include <stack>
#include <iostream>

int main() {
    // Create a stack of integers
    std::stack<int> stack;
    
    // Push elements
    stack.push(1);
    stack.push(2);
    stack.push(3);
    
    // Peek at top element
    std::cout << "Top element: " << stack.top() << std::endl;  // Output: 3
    
    // Check size
    std::cout << "Stack size: " << stack.size() << std::endl;  // Output: 3
    
    // Pop and print elements
    while (!stack.empty()) {
        std::cout << stack.top() << " ";  // Output: 3 2 1
        stack.pop();
    }
    
    return 0;
}
```

## Advantages

- Simple and easy to implement
- Memory efficient
- Fast operations (constant time)

## Limitations

- Fixed size in some implementations
- No random access to elements
- Can only access the top element

## Related Data Structures

- Queue (FIFO principle)
- Deque (Double-ended queue)
- Priority Queue

## Real-world Applications

- Memory management in operating systems
- Expression evaluation in calculators
- Recursion implementation
- Parsing in compilers
- Browser history tracking

## References

- [Stack](https://en.wikipedia.org/wiki/Stack_(data_structure))
