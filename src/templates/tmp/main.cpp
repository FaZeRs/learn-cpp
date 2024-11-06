#include <print>
#include <type_traits>

// Base case: factorial of 0 is 1
template <int N>
struct Factorial : std::integral_constant<int, N * Factorial<N - 1>::value> {};

// Specialization for 0
template <>
struct Factorial<0> : std::integral_constant<int, 1> {};

int main() {
  // Compute factorial of 5 at compile time
  std::println("Factorial of 5 is {}", Factorial<5>::value);

  // Compute factorial of 10 at compile time
  std::println("Factorial of 10 is {}", Factorial<10>::value);

  return 0;
}