#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

template <typename T>
class Node {
 public:
  T data;
  std::unique_ptr<Node<T>> next{nullptr};
  explicit Node(const T &val) : data(val) {}
};

template <typename T>
class LinkedList {
 public:
  LinkedList() = default;
  ~LinkedList() = default;
  LinkedList(const LinkedList &) = default;
  LinkedList &operator=(const LinkedList &) = default;
  LinkedList(LinkedList &&) noexcept = default;
  LinkedList &operator=(LinkedList &&) noexcept = default;

  void push_back(T val) {
    auto new_node = std::make_unique<Node<T>>(val);
    if (!m_head) {
      m_head = std::move(new_node);
      return;
    }

    auto current = m_head.get();
    while (current->next) {
      current = current->next.get();
    }
    current->next = std::move(new_node);
  }

  void push_front(T val) {
    auto new_node = std::make_unique<Node<T>>(val);
    if (!m_head) {
      m_head = std::move(new_node);
      return;
    }

    new_node->next = std::move(m_head);
    m_head = std::move(new_node);
  }

  void insert(int pos, T val) {
    auto new_node = std::make_unique<Node<T>>(val);
    if (!m_head) {
      m_head = std::move(new_node);
      return;
    }

    auto current = m_head.get();
    int i = 0;
    while (current->next && i < pos - 1) {
      current = current->next.get();
      i++;
    }
    new_node->next = std::move(current->next);
    current->next = std::move(new_node);
  }

  T *at(int pos) {
    if (!m_head) {
      return nullptr;
    }

    auto current = m_head.get();
    int i = 0;
    while (current->next && i < pos) {
      current = current->next.get();
      i++;
    }
    return &current->data;
  }

  void erase(int pos) {
    if (!m_head) {
      return;
    }

    auto current = m_head.get();
    int i = 0;
    while (current->next && i < pos - 1) {
      current = current->next.get();
      i++;
    }
    current->next = std::move(current->next->next);
  }

  void print() const {
    auto current = m_head.get();
    while (current) {
      std::cout << current->data << " ";
      current = current->next.get();
    }
    std::cout << std::endl;
  }

 private:
  std::unique_ptr<Node<T>> m_head{nullptr};
};

int main() {
  LinkedList<int> list;
  list.push_back(5);
  list.push_back(10);
  list.push_back(15);

  std::cout << "Linked List Elements: ";
  list.print();

  list.insert(1, 20);
  std::cout << "Linked List Elements: ";
  list.print();

  std::cout << "Element at position 2: " << *list.at(2) << std::endl;

  list.push_front(25);
  std::cout << "Linked List Elements: ";
  list.print();

  list.erase(1);
  std::cout << "Linked List Elements: ";
  list.print();

  return EXIT_SUCCESS;
}
