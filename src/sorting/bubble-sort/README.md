# Bubble Sort

Bubble Sort is a simple sorting algorithm that repeatedly steps through a list, compares adjacent elements, and swaps them if they are in the wrong order. The pass through the list is repeated until no more swaps are needed.

## Algorithm Description

1. Start with an unsorted array
2. Compare adjacent elements, swapping them if they are in the wrong order
3. After each pass, the largest unsorted element "bubbles up" to its correct position
4. Repeat until no swaps are needed in a pass

## Complexity

- Time Complexity: 
  - Best Case: O(n) when array is already sorted
  - Average Case: O(n²)
  - Worst Case: O(n²)
- Space Complexity: O(1)

## Advantages and Disadvantages

### Advantages
- Simple to understand and implement
- Requires no additional memory space
- Stable sorting algorithm (maintains relative order of equal elements)

### Disadvantages
- Poor performance with O(n²) complexity
- Not suitable for large datasets
- Not very efficient compared to other sorting algorithms like Quick Sort or Merge Sort

## When to Use Bubble Sort

Bubble Sort is best used when:
- The array size is small
- Memory space is limited
- The code needs to be simple and easy to understand
- The array is nearly sorted
- A stable sort is required

## Visual Representation

Initial Array: [5, 3, 8, 4, 2]

Pass 1: [3, 5, 4, 2, 8]
Pass 2: [3, 4, 2, 5, 8]
Pass 3: [3, 2, 4, 5, 8]
Pass 4: [2, 3, 4, 5, 8]
