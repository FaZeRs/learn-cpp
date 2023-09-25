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
class Queue {
 public:
  Queue() = default;
  ~Queue() = default;
  Queue(const Queue &) = default;
  Queue &operator=(const Queue &) = default;
  Queue(Queue &&) noexcept = default;
  Queue &operator=(Queue &&) noexcept = default;

  [[nodiscard]] bool isEmpty() const { return m_front == nullptr; }

  void enqueue(T val) {
    auto newNode = std::make_unique<Node<T>>(val);
    if (isEmpty()) {
      m_front = std::move(newNode);
      m_rear = m_front.get();
    } else {
      m_rear->next = std::move(newNode);
      m_rear = m_rear->next.get();
    }
  }

  bool dequeue() {
    if (isEmpty()) {
      std::cerr << "Queue is empty\n";
      return false;
    }
    m_front = std::move(m_front->next);
    if (m_front == nullptr) m_rear = nullptr;
    return true;
  }

  void display() const {
    auto current = m_front.get();
    while (current != nullptr) {
      std::cout << current->data << " ";
      current = current->next.get();
    }
    std::cout << "\n";
  }

 private:
  std::unique_ptr<Node<T>> m_front{nullptr};
  Node<T> *m_rear{nullptr};
};

int main() {
  Queue<int> q;
  q.enqueue(10);
  q.enqueue(20);
  q.enqueue(30);
  q.enqueue(40);

  std::cout << "Queue elements: ";
  q.display();

  q.dequeue();
  std::cout << "Queue elements after dequeue: ";
  q.display();

  return 0;
}
