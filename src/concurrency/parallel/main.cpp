#include <execution>
#include <functional>
#include <numeric>
#include <print>
#include <vector>

constexpr auto sum_range = [](auto start, auto end) -> int {
  return (end - start) * (start + end - 1) / 2;
};

int main() {
  constexpr int n = 1000;         // Range to sum up to
  constexpr int num_threads = 4;  // Number of threads

  // Calculate the range each thread will handle
  constexpr auto range_per_thread = n / num_threads;
  std::vector<int> indices(num_threads);
  std::ranges::iota(indices, 0);

  // Summing up the partial sums in parallel
  const int total_sum = std::transform_reduce(
      std::execution::par, indices.begin(), indices.end(), 0, std::plus<>(),
      [&](int i) {
        auto start = i * range_per_thread + 1;
        auto end =
            (i == num_threads - 1) ? (n + 1) : (start + range_per_thread);
        return sum_range(start, end);
      });

  std::println("Total sum from 1 to {} is: {}", n, total_sum);

  return 0;
}
