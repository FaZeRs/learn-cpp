#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>

template <typename T>
class BinaryTree {
public:
    struct Node {
        T data;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

        explicit Node(T value) : data(value), left(nullptr), right(nullptr) {}
    };

    BinaryTree() : root(nullptr) {}

    void insert(const T& value) {
        if (!root) {
            root = std::make_unique<Node>(value);
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
    std::unique_ptr<Node> root;

    void insert(const T& value, Node* node) {
        if (value < node->data) {
            if (!node->left) {
                node->left = std::make_unique<Node>(value);
            } else {
                insert(value, node->left.get());
            }
        } else if (value > node->data) {
            if (!node->right) {
                node->right = std::make_unique<Node>(value);
            } else {
                insert(value, node->right.get());
            }
        }
    }

    void inOrderTraversal(const Node* node) const {
        if (!node) return;
        inOrderTraversal(node->left.get());
        std::cout << node->data << ' ';
        inOrderTraversal(node->right.get());
    }

    void preOrderTraversal(const Node* node) const {
        if (!node) return;
        std::cout << node->data << ' ';
        preOrderTraversal(node->left.get());
        preOrderTraversal(node->right.get());
    }

    void postOrderTraversal(const Node* node) const {
        if (!node) return;
        postOrderTraversal(node->left.get());
        postOrderTraversal(node->right.get());
        std::cout << node->data << ' ';
    }

    bool search(const T& value, const Node* node) const {
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
