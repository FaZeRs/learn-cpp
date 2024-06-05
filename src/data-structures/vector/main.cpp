#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

template <typename T>
class Vector {
 public:
  Vector() {
    m_data = std::make_unique<T[]>(initial_capacity);  // NOLINT
  }
  ~Vector() = default;
  Vector(const Vector &) = default;
  Vector &operator=(const Vector &) = default;
  Vector(Vector &&) noexcept = default;
  Vector &operator=(Vector &&) noexcept = default;

  [[nodiscard]] constexpr T &operator[](size_t index) {
    if (index >= m_size) throw std::out_of_range("Vector index out of range");
    return m_data[index];  // NOLINT
  }
  [[nodiscard]] constexpr const T &operator[](size_t index) const {
    if (index >= m_size) throw std::out_of_range("Vector index out of range");
    return m_data[index];
  }

  void push_back(const T &value) {
    if (m_size == m_capacity) reserve(m_capacity * growth_factor);

    m_data[m_size++] = value;
  }

  void emplace_back(T &&value) {
    if (m_size == m_capacity) reserve(m_capacity * growth_factor);

    m_data[m_size++] = std::move(value);  // NOLINT
  }

  [[nodiscard]] size_t size() const { return m_size; }

  T *begin() noexcept { return m_data.get(); }
  [[nodiscard]] const T *begin() const noexcept { return m_data.get(); }
  T *end() noexcept {
    // NOLINTNEXTLINE
    return static_cast<T *>(m_data.get()) + m_size;
  }
  [[nodiscard]] const T *end() const noexcept {
    return static_cast<const T *>(m_data.get()) + m_size;
  }

  [[nodiscard]] T *data() noexcept { return m_data.get(); }
  [[nodiscard]] const T *data() const noexcept { return m_data.get(); }

 private:
  void reserve(size_t new_capacity) {
    if (new_capacity <= m_capacity) return;

    // NOLINTNEXTLINE
    std::unique_ptr<T[]> new_data = std::make_unique<T[]>(new_capacity);
    for (size_t i = 0; i < m_size; i++) {
      new_data[i] = m_data[i];
    }

    m_data = std::move(new_data);
    m_capacity = new_capacity;
  }

  static constexpr size_t initial_capacity = 4;
  static constexpr size_t growth_factor = 2;

  size_t m_size{};
  size_t m_capacity{initial_capacity};
  std::unique_ptr<T[]> m_data;  // NOLINT
};

int main() {
  Vector<int> vec;
  vec.emplace_back(1);
  vec.emplace_back(2);
  vec.emplace_back(3);

  for (const auto &item : vec) {
    std::cout << item << "\n";
  }

  return EXIT_SUCCESS;
}
