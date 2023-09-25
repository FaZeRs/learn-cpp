#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

template <typename T, size_t S>
class Array {
 public:
  Array() = default;
  ~Array() = default;
  Array(const Array &) = default;
  Array &operator=(const Array &) = default;
  Array(Array &&) noexcept = default;
  Array &operator=(Array &&) noexcept = default;

  [[nodiscard]] constexpr T &operator[](size_t index) {
    if (index >= S) throw std::out_of_range("Index out of range");
    return m_data[index];  // NOLINT
  }
  [[nodiscard]] constexpr const T &operator[](size_t index) const {
    if (index >= S) throw std::out_of_range("Index out of range");
    return m_data[index];
  }

  T *data() { return m_data; }
  [[nodiscard]] const T *data() const { return m_data; }

  [[nodiscard]] static constexpr size_t size() noexcept { return S; }

  T *begin() noexcept { return m_data; }  // NOLINT
  [[nodiscard]] const T *begin() const noexcept { return m_data; }
  T *end() noexcept { return m_data + S; }  // NOLINT
  [[nodiscard]] const T *end() const noexcept { return m_data + S; }

 private:
  T m_data[S];  // NOLINT
};

int main() {
  Array<std::string, 5> data;
  data[0] = "Hello";
  data[1] = "World";
  data[2] = "!";
  data[3] = "I'm a";
  data[4] = "string";
  for (const auto &item : data) {
    std::cout << item << std::endl;
  }

  Array<int, 2> data2{};
  data2[0] = 2;
  data2[1] = 1;

  for (const auto &item : data2) {
    std::cout << item << std::endl;
  }

  return EXIT_SUCCESS;
}
