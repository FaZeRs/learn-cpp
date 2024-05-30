#include <iostream>
#include <memory>

template <typename T>
class CircularLinkedList {
 public:
  CircularLinkedList() = default;
  ~CircularLinkedList() = default;
  CircularLinkedList(const CircularLinkedList &) = default;
  CircularLinkedList &operator=(const CircularLinkedList &) = default;
  CircularLinkedList(CircularLinkedList &&) noexcept = default;
  CircularLinkedList &operator=(CircularLinkedList &&) noexcept = default;

  void push_back(T val) {
    auto new_node = std::make_shared<Node>(val);
    if (!m_head) {
      m_head = std::move(new_node);
      m_head->next = m_head;
      return;
    }

    auto current = m_head.get();
    while (current->next != m_head) {
      current = current->next.get();
    }
    new_node->next = m_head;
    current->next = std::move(new_node);
  }

  void push_front(T val) {
    auto new_node = std::make_shared<Node>(val);
    if (!m_head) {
      m_head = std::move(new_node);
      m_head->next = m_head;
      return;
    }

    auto temp = m_head;
    while (temp->next != m_head) {
      temp = temp->next;
    }

    new_node->next = std::move(m_head);
    m_head = std::move(new_node);
    temp->next = m_head;
  }

  void insert(int pos, T val) {
    auto new_node = std::make_shared<Node>(val);
    if (!m_head) {
      m_head = std::move(new_node);
      m_head->next = m_head;
      return;
    }

    auto current = m_head.get();
    int i = 0;
    while (current->next != m_head && i < pos - 1) {
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
    while (current->next != m_head && i < pos) {
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
    while (current->next != m_head && i < pos - 1) {
      current = current->next.get();
      i++;
    }

    current->next = std::move(current->next->next);
  }

  void pop_front() {
    if (!m_head) {
      return;
    }

    if (m_head->next == m_head) {
      m_head = nullptr;
      return;
    }

    auto temp = m_head;
    while (temp->next != m_head) {
      temp = temp->next;
    }

    m_head = std::move(m_head->next);
    temp->next = m_head;
  }

  void pop_back() {
    if (!m_head) {
      return;
    }

    if (m_head->next == m_head) {
      m_head = nullptr;
      return;
    }

    auto current = m_head;
    while (current->next->next != m_head) {
      current = current->next;
    }
    current->next = std::move(current->next->next);
  }

  void print() const {
    if (!m_head) {
      return;
    }
    auto current = m_head;
    while (current->next != m_head) {
      std::cout << current->data << " ";
      current = current->next;
    }
    std::cout << current->data << " ";
    std::cout << std::endl;
  }

  [[nodiscard]] size_t size() const {
    if (!m_head) {
      return 0;
    }
    size_t count = 1;
    auto current = m_head;
    while (current->next != m_head) {
      count++;
      current = current->next;
    }
    return count;
  }

 private:
  struct Node {
    T data;
    std::shared_ptr<Node> next{nullptr};
    explicit Node(const T &val) : data(val) {}
  };

  std::shared_ptr<Node> m_head{nullptr};
};
