#include <cstdlib>
#include <latch>
#include <numeric>
#include <print>
#include <ranges>
#include <thread>
#include <vector>

template <typename T>
concept Integral = std::is_integral_v<T>;

constexpr auto sum_range = [](Integral auto start,
                              Integral auto end) -> Integral auto {
  return (end - start) * (start + end - 1) / 2;
};

int main() {
  constexpr int n = 1000;         // Range to sum up to
  constexpr int num_threads = 4;  // Number of threads

  // Calculate the range each thread will handle
  constexpr int range_per_thread = n / num_threads;
  std::vector<int> results(static_cast<size_t>(num_threads));
  std::latch latch(num_threads);

  // Create and start threads
  std::vector<std::jthread> threads;
  for (const int i : std::views::iota(0, num_threads)) {
    const int start = i * range_per_thread + 1;
    const int end =
        (i == num_threads - 1) ? (n + 1) : (start + range_per_thread);
    threads.emplace_back([&results, start, end, i, &latch]() {
      results[static_cast<size_t>(i)] = sum_range(start, end);
      latch.count_down();
    });
  }

  // Wait for all threads to finish
  latch.wait();

  // Combine results from all threads
  const int total_sum = std::accumulate(results.begin(), results.end(), 0);

  std::println("Total sum from 1 to {} is: {}", n, total_sum);

  return 0;
}