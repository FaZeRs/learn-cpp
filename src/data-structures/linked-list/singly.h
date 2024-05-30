#include <iostream>
#include <memory>

template <typename T>
class SinglyLinkedList {
 public:
  SinglyLinkedList() = default;
  ~SinglyLinkedList() = default;
  SinglyLinkedList(const SinglyLinkedList &) = default;
  SinglyLinkedList &operator=(const SinglyLinkedList &) = default;
  SinglyLinkedList(SinglyLinkedList &&) noexcept = default;
  SinglyLinkedList &operator=(SinglyLinkedList &&) noexcept = default;

  void push_back(T val) {
    auto new_node = std::make_unique<Node>(val);
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
    auto new_node = std::make_unique<Node>(val);
    if (!m_head) {
      m_head = std::move(new_node);
      return;
    }

    new_node->next = std::move(m_head);
    m_head = std::move(new_node);
  }

  void insert(int pos, T val) {
    auto new_node = std::make_unique<Node>(val);
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

  void pop_front() {
    if (!m_head) {
      return;
    }

    m_head = std::move(m_head->next);
  }

  void pop_back() {
    if (!m_head) {
      return;
    }

    auto current = m_head.get();
    while (current->next->next) {
      current = current->next.get();
    }
    current->next = nullptr;
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
    explicit Node(const T &val) : data(val) {}
  };

  std::unique_ptr<Node> m_head{nullptr};
};
