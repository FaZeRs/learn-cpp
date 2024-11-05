# Ternary Search

Ternary search is a divide-and-conquer search algorithm that can be used to find the minimum or maximum point in a unimodal function, or to locate a specific element in a sorted array. It works by dividing the search space into three parts rather than two (as in binary search).

## How it Works

1. Instead of dividing the array into two parts like binary search, ternary search divides the array into three parts.
2. It compares the target value with the values at two partition points.
3. Based on these comparisons, it eliminates one-third of the array in each iteration.

## Time Complexity

- **Best Case**: O(1)
- **Average Case**: O(log₃ n)
- **Worst Case**: O(log₃ n)

## Space Complexity

- O(1) for iterative implementation
- O(log₃ n) for recursive implementation due to the call stack

## Advantages

- More efficient than binary search for finding maximum/minimum points in unimodal functions
- Can be used to find local maxima/minima in continuous functions
- Useful in optimization problems

## Disadvantages

- Slightly slower than binary search for finding specific elements
- Requires more comparisons per iteration than binary search
- Only works on sorted arrays or unimodal functions

## Use Cases

1. Finding the maximum or minimum point in a unimodal function
2. Optimization problems in numerical analysis
3. Finding specific elements in a sorted array (though binary search is typically preferred for this)

## References

- [Wikipedia - Ternary Search](https://en.wikipedia.org/wiki/Ternary_search)
- [GeeksforGeeks - Ternary Search](https://www.geeksforgeeks.org/ternary-search/)