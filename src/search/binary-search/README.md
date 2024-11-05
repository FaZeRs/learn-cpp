# Binary Search

Binary Search is an efficient searching algorithm that finds the position of a target value within a sorted array. It works by repeatedly dividing the search interval in half.

## Time Complexity
- **Best Case**: O(1)
- **Average Case**: O(log n)
- **Worst Case**: O(log n)

## Space Complexity
- O(1) - iterative implementation
- O(log n) - recursive implementation due to call stack

## How It Works
1. Compare the target value with the middle element of the array
2. If the target matches the middle element, return its position
3. If the target is less than the middle element, search the left half
4. If the target is greater than the middle element, search the right half
5. Repeat steps 1-4 until the target is found or determined to not exist

## Advantages
- Very efficient for large sorted datasets
- Logarithmic time complexity makes it much faster than linear search
- Simple to implement

## Disadvantages
- Array must be sorted first
- Only works with sequential access data structures (arrays)
- Not suitable for small datasets where linear search might be faster

## Use Cases
- Finding an element in a sorted array
- Dictionary lookup
- Phone book search
- Database indexing
- Finding insertion point for a new element in sorted array

## Implementation Notes
- Ensure array is sorted before using binary search
- Handle edge cases (empty array, single element)
- Consider overflow when calculating middle index
- Account for duplicates if necessary

## Related Algorithms
- Linear Search
- Jump Search
- Interpolation Search
- Exponential Search

## References

- [GeeksforGeeks - Binary Search](https://www.geeksforgeeks.org/binary-search/)
- [Wikipedia - Binary Search](https://en.wikipedia.org/wiki/Binary_search_algorithm)
