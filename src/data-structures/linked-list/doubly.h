#include <iostream>
#include <memory>
#include <stdexcept>

template <typename T>
class DoublyLinkedList {
 public:
  DoublyLinkedList() = default;
  ~DoublyLinkedList() = default;
  DoublyLinkedList(const DoublyLinkedList &) = default;
  DoublyLinkedList &operator=(const DoublyLinkedList &) = default;
  DoublyLinkedList(DoublyLinkedList &&) noexcept = default;
  DoublyLinkedList &operator=(DoublyLinkedList &&) noexcept = default;

  void push_back(const T& val) {
    auto new_node = std::make_unique<Node>(val);
    if (!m_head) {
      m_head = std::move(new_node);
      m_tail = m_head.get();
      return;
    }

    new_node->prev = m_tail;
    m_tail->next = std::move(new_node);
    m_tail = m_tail->next.get();
  }

  void push_front(const T& val) {
    auto new_node = std::make_unique<Node>(val);
    if (!m_head) {
      m_head = std::move(new_node);
      m_tail = m_head.get();
      return;
    }

    new_node->next = std::move(m_head);
    m_head = std::move(new_node);
    m_head->next->prev = m_head.get();
  }

  void insert(size_t pos, const T& val) {
    if (pos == 0) {
      push_front(val);
      return;
    }

    auto new_node = std::make_unique<Node>(val);
    if (!m_head) {
      m_head = std::move(new_node);
      m_tail = m_head.get();
      return;
    }

    auto current = m_head.get();
    size_t i = 0;
    while (current && i < pos - 1) {
      current = current->next.get();
      i++;
    }
    if (!current) {
      throw std::out_of_range("Index out of range");
    }
    new_node->next = std::move(current->next);
    if (new_node->next) {
      new_node->next->prev = new_node.get();
    }
    current->next = std::move(new_node);
    current->next->prev = current;
  }

  void print() const {
    if (!m_head) {
      return;
    }
    auto current = m_head.get();
    while (current) {
      std::cout << current->data << " ";
      current = current->next.get();
    }
    std::cout << std::endl;
  }

  T *at(int pos) {
    if (!m_head || pos < 0) {
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
    if (!m_head || pos < 0) {
      return;
    }

    auto current = m_head.get();
    int i = 0;
    while (current->next && i < pos) {
      current = current->next.get();
      i++;
    }
    if (!current) {
      throw std::out_of_range("Index out of range");
    }

    if (current->next) {
      current->next->prev = current->prev;
    } else {
      m_tail = current->prev;
    }

    if (current->prev) {
      current->prev->next = std::move(current->next);
    } else {
      m_head = std::move(current->next);
    }
  }

  void pop_front() {
    if (!m_head) {
      return;
    }

    if (m_head->next) {
      m_head->next->prev = nullptr;
    } else {
      m_tail = nullptr;
    }
    m_head = std::move(m_head->next);
  }

  void pop_back() {
    if (!m_head) {
      return;
    }

    if (m_tail->prev) {
      m_tail->prev->next = nullptr;
    } else {
      m_head = nullptr;
    }
    m_tail = m_tail->prev;
  }

  [[nodiscard]] size_t size() const {
    if (!m_head) {
      return 0;
    }
    size_t count = 0;
    auto current = m_head.get();
    while (current) {
      count++;
      current = current->next.get();
    }
    return count;
  }

 private:
  struct Node {
    T data;
    std::unique_ptr<Node> next{nullptr};
    Node* prev{nullptr};
    explicit Node(const T &val) : data(val) {}
  };

  std::unique_ptr<Node> m_head{nullptr};
  Node* m_tail{nullptr};
};
