# Binary Tree

A binary tree is a hierarchical data structure composed of nodes, where each node contains a value and has up to two children, referred to as the left child and the right child.

## Basic Structure

```text
     1
   /   \
  2     3
 / \   / \
4   5 6   7
```

## Properties

- Each node has at most two children (left and right)
- Each node contains a value/data
- The left subtree of a node contains only nodes with values less than the node's value (in a Binary Search Tree)
- The right subtree of a node contains only nodes with values greater than the node's value (in a Binary Search Tree)

## Types of Binary Trees

1. **Full Binary Tree**: Every node has either 0 or 2 children
2. **Complete Binary Tree**: All levels are filled except possibly the last level, which is filled from left to right
3. **Perfect Binary Tree**: All internal nodes have two children and all leaves are at the same level
4. **Binary Search Tree (BST)**: Left subtree contains nodes with smaller values, right subtree contains larger values
5. **Balanced Binary Tree**: Height difference between left and right subtrees for every node is at most one

## Common Operations

- **Insertion**: Add a new node to the tree
- **Deletion**: Remove a node from the tree
- **Traversal**: Visit all nodes in a specific order
  - In-order (Left, Root, Right)
  - Pre-order (Root, Left, Right)
  - Post-order (Left, Right, Root)
  - Level-order (Breadth-first)
- **Search**: Find a specific value in the tree
- **Height**: Calculate the maximum depth of the tree

## Time Complexity

| Operation | Average Case | Worst Case |
|-----------|-------------|------------|
| Access    | O(log n)    | O(n)       |
| Search    | O(log n)    | O(n)       |
| Insertion | O(log n)    | O(n)       |
| Deletion  | O(log n)    | O(n)       |

## Space Complexity

- O(n) for storing n nodes
- O(h) for recursive operations where h is the height of the tree

## Use Cases

1. File system organization
2. Expression parsing
3. Database indexing
4. Decision trees in machine learning
5. Syntax trees in compilers
6. Hierarchical data representation

## Implementation

Basic node structure in C++:

```C++
class Node {
public:
    int value;
    Node* left;
    Node* right;
    
    Node(int val) : value(val), left(nullptr), right(nullptr) {}
};
```

## Advantages

- Provides a natural way to represent hierarchical relationships
- Efficient searching and sorting (in case of BST)
- Dynamic size
- Easy to understand and implement

## Disadvantages

- May become unbalanced, leading to poor performance
- No constant-time access to elements
- Requires proper maintenance for optimal performance

## Related Data Structures

- AVL Trees
- Red-Black Trees
- B-Trees
- Binary Heaps

## References

- [Binary Trees on GeeksforGeeks](https://www.geeksforgeeks.org/binary-tree-data-structure/)
- [Binary Search Trees on Wikipedia](https://en.wikipedia.org/wiki/Binary_search_tree)
