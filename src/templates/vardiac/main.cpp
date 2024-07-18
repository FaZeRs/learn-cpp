#include <cstddef>
#include <print>
#include <tuple>
#include <utility>
#include <variant>

template <typename T>
void print(const T& t) {
  std::println("{}", t);
}

template <typename T, typename... Args>
void print(const T& t, const Args&... args) {
  std::print("{} ", t);
  print(args...);
}

template <typename... Types>
class MyTuple {
 public:
  explicit MyTuple(Types... args) : m_data(args...) {}

  void print() const { printTuple(m_data); }

 private:
  std::tuple<Types...> m_data;

  template <std::size_t... Is>
  void printTupleHelper(const std::tuple<Types...>& t,
                        std::index_sequence<Is...>) const {
    ((std::print("{} ", std::get<Is>(t))), ...);
  }

  void printTuple(const std::tuple<Types...>& t) const {
    printTupleHelper(t, std::index_sequence_for<Types...>{});
  }
};

int main() {
  print(1, 2.5, "Hello", 'c', 42);

  MyTuple my_tuple(1, 2.5, "Hello");
  my_tuple.print();
  return 0;
}