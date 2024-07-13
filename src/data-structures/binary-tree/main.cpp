#include <iostream>
#include <memory>

template <typename T>
struct Node {
  T data;
  std::unique_ptr<Node<T>> left{nullptr};
  std::unique_ptr<Node<T>> right{nullptr};
  explicit Node(const T& value) : data(value) {}
};

template <typename T>
class BinaryTree {
 public:
  void insert(const T& value) {
    if (!root) {
      root = std::make_unique<Node<T>>(value);
    } else {
      insert(value, root.get());
    }
  }

  void inOrderTraversal() const {
    inOrderTraversal(root.get());
    std::cout << '\n';
  }

  void preOrderTraversal() const {
    preOrderTraversal(root.get());
    std::cout << '\n';
  }

  void postOrderTraversal() const {
    postOrderTraversal(root.get());
    std::cout << '\n';
  }

  [[nodiscard]] bool search(const T& value) const {
    return search(value, root.get());
  }

 private:
  std::unique_ptr<Node<T>> root{nullptr};

  void insert(const T& value, Node<T>* node) {
    if (value < node->data) {
      if (!node->left) {
        node->left = std::make_unique<Node<T>>(value);
      } else {
        insert(value, node->left.get());
      }
    } else if (value > node->data) {
      if (!node->right) {
        node->right = std::make_unique<Node<T>>(value);
      } else {
        insert(value, node->right.get());
      }
    }
  }

  static void inOrderTraversal(const Node<T>* node) {
    if (!node) return;
    inOrderTraversal(node->left.get());
    std::cout << node->data << ' ';
    inOrderTraversal(node->right.get());
  }

  static void preOrderTraversal(const Node<T>* node) {
    if (!node) return;
    std::cout << node->data << ' ';
    preOrderTraversal(node->left.get());
    preOrderTraversal(node->right.get());
  }

  static void postOrderTraversal(const Node<T>* node) {
    if (!node) return;
    postOrderTraversal(node->left.get());
    postOrderTraversal(node->right.get());
    std::cout << node->data << ' ';
  }

  static bool search(const T& value, const Node<T>* node) {
    if (!node) return false;
    if (value == node->data) return true;
    if (value < node->data) return search(value, node->left.get());
    return search(value, node->right.get());
  }
};

int main() {
  BinaryTree<int> bt;
  bt.insert(5);
  bt.insert(2);
  bt.insert(8);
  bt.insert(1);
  bt.insert(3);

  std::cout << "In-order Traversal: ";
  bt.inOrderTraversal();
  std::cout << "Pre-order Traversal: ";
  bt.preOrderTraversal();
  std::cout << "Post-order Traversal: ";
  bt.postOrderTraversal();

  std::cout << "Search 3: " << (bt.search(3) ? "Found" : "Not Found") << '\n';
  std::cout << "Search 4: " << (bt.search(4) ? "Found" : "Not Found") << '\n';

  return 0;
}
