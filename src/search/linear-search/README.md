# Linear Search

Linear Search (also called Sequential Search) is the simplest searching algorithm that searches for an element in a list by checking each element sequentially until a match is found or the entire list has been searched.

## Algorithm Description

The algorithm works by:
1. Starting from the first element of the list
2. Comparing each element with the target value
3. If a match is found, return the index of the element
4. If no match is found after checking all elements, return -1

## Complexity

- Time Complexity: O(n)
  - Best Case: O(1) - Element found at first position
  - Average Case: O(n/2)
  - Worst Case: O(n) - Element found at last position or not found

- Space Complexity: O(1)

## Advantages and Disadvantages

### Advantages
- Simple and easy to implement
- No preprocessing required
- Works on unsorted arrays
- Good for small datasets
- Memory efficient

### Disadvantages
- Slow for large datasets
- Not suitable for large sorted arrays (Binary Search would be better)

## Use Cases

- Searching in small unsorted lists
- Searching in lists that are rarely accessed
- When simplicity is preferred over efficiency
- When the list is too small to justify more complex algorithms
- When the list is likely to be searched only once

## Real-world Applications

1. Finding a contact in an unsorted phone book
2. Searching for a specific item in a small inventory
3. Looking for a specific element in a small dataset
4. Finding a specific process in a task manager

## References

- [GeeksforGeeks - Linear Search](https://www.geeksforgeeks.org/linear-search/)
- [Wikipedia - Linear Search](https://en.wikipedia.org/wiki/Linear_search)
