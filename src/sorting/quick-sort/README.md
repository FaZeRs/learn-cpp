# Quick Sort

Quick Sort is a highly efficient, comparison-based sorting algorithm that uses a divide-and-conquer strategy. It was developed by Tony Hoare in 1959 and is still one of the most widely used sorting algorithms.

## Algorithm Overview

Quick Sort works by:
1. Selecting a 'pivot' element from the array
2. Partitioning other elements into two sub-arrays according to whether they are less than or greater than the pivot
3. Recursively sorting the sub-arrays

## Time Complexity

- Best Case: O(n log n)
- Average Case: O(n log n)
- Worst Case: O(n²)

## Space Complexity

- O(log n) - due to the recursive call stack

## Advantages

- Fast in practice
- In-place sorting (requires small additional space)
- Cache friendly
- Can be easily modified for different types of data

## Disadvantages

- Unstable sort (doesn't preserve relative order of equal elements)
- O(n²) worst-case complexity
- Not adaptive (performance doesn't improve with partially sorted arrays)

## Use Cases

Quick Sort is particularly useful when:
- You need a fast, general-purpose sorting algorithm
- Memory space is a concern
- Average-case performance is more important than worst-case guarantees
- Random access to elements is efficient (like in arrays)

## Implementation Notes

- The choice of pivot can significantly affect performance
- Common pivot selection strategies include:
  - First element
  - Last element
  - Middle element
  - Random element
  - Median-of-three

## References

- [Quick Sort on Wikipedia](https://en.wikipedia.org/wiki/Quicksort)
