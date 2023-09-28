#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

template <typename T>
struct Node {
  T data;
  std::unique_ptr<Node<T>> next{nullptr};
  explicit Node(const T &val) : data(val) {}
};

template <typename T>
class Stack {
 public:
  Stack() = default;
  ~Stack() = default;
  Stack(const Stack &) = default;
  Stack &operator=(const Stack &) = default;
  Stack(Stack &&) noexcept = default;
  Stack &operator=(Stack &&) noexcept = default;

  void push(const T &data) {
    auto new_node = std::make_unique<Node<T>>(data);
    new_node->next = std::move(m_top);
    m_top = std::move(new_node);
    m_size++;
  }

  void pop() {
    if (isEmpty()) {
      throw std::out_of_range("Stack is empty!");
    }

    m_top = std::move(m_top->next);
    m_size--;
  }

  T &top() {
    if (isEmpty()) {
      throw std::out_of_range("Stack is empty!");
    }
    return m_top->data;
  }

  [[nodiscard]] bool isEmpty() const { return m_top == nullptr; }
  [[nodiscard]] size_t size() const { return m_size; }

 private:
  std::unique_ptr<Node<T>> m_top{nullptr};
  size_t m_size{0};
};

int main() {
  Stack<int> s;

  s.push(1);
  s.push(2);
  s.push(3);

  std::cout << "Top: " << s.top() << std::endl;  // 3
  s.pop();
  std::cout << "Top after pop: " << s.top() << std::endl;  // 2
  std::cout << "Size: " << s.size() << std::endl;          // 2

  return EXIT_SUCCESS;
}
