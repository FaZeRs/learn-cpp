# Queue

A Queue is a linear data structure that follows the First In First Out (FIFO) principle. Elements are added at the rear (enqueue) and removed from the front (dequeue) of the queue.

## Basic Operations

- **Enqueue**: Add an element to the rear of the queue
- **Dequeue**: Remove and return the element at the front of the queue
- **Peek/Front**: View the first element without removing it
- **IsEmpty**: Check if the queue is empty
- **Size**: Get the number of elements in the queue

## Time Complexity

| Operation | Time Complexity |
|-----------|----------------|
| Enqueue   | O(1)          |
| Dequeue   | O(1)          |
| Peek      | O(1)          |
| IsEmpty   | O(1)          |
| Size      | O(1)          |

## Common Applications

1. Task Scheduling
2. Print Queue Management
3. Breadth-First Search
4. Buffer for Data Streams
5. Message Queue Systems

## Implementation Types

1. **Array-based Queue**
   - Fixed size
   - Simple implementation
   - Memory efficient

2. **Linked List Queue**
   - Dynamic size
   - Memory overhead for node pointers
   - Better for frequent enqueue/dequeue

3. **Circular Queue**
   - Efficient memory utilization
   - Prevents queue overflow in fixed-size implementations
   - Useful for continuous processing

## Code Example

```c++
#include <iostream>
#include <queue>
#include <string>

int main() {
    struct Task {
        int id;
        std::string name;
        Task(int i, std::string n) : id(i), name(n) {}
    };

    std::queue<Task> task_queue;
    task_queue.push(Task(1, "Write code"));
    task_queue.push(Task(2, "Review PR"));
    task_queue.push(Task(3, "Fix bugs"));

    // Process all tasks
    while (!task_queue.empty()) {
        Task current_task = task_queue.front();
        std::cout << "Processing Task - ID: " << current_task.id 
                  << ", Name: " << current_task.name << "\n";
        task_queue.pop();
    }
}
```

## Common Problems and Solutions

1. **Queue Overflow**
   - Solution: Use dynamic sizing or circular queue
   
2. **Memory Wastage**
   - Solution: Implement circular queue or linked list-based queue

3. **Performance Issues**
   - Solution: Choose appropriate implementation based on use case

## Related Data Structures

- Priority Queue
- Deque (Double-ended queue)
- Circular Queue

## References

- [GeeksforGeeks - Queue Data Structure](https://www.geeksforgeeks.org/queue-data-structure/)
- [Wikipedia - Queue (abstract data type)](https://en.wikipedia.org/wiki/Queue_(abstract_data_type))
