#include <omp.h>

#include <cstdlib>
#include <numeric>
#include <print>
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

#pragma omp parallel num_threads(num_threads) default(none) \
    shared(results, n, range_per_thread, sum_range)
  {
    int thread_id = omp_get_thread_num();
    const int start = thread_id * range_per_thread + 1;
    const int end =
        (thread_id == num_threads - 1) ? (n + 1) : (start + range_per_thread);
    results[static_cast<size_t>(thread_id)] = sum_range(start, end);
  }

  // Combine results from all threads
  const int total_sum = std::accumulate(results.begin(), results.end(), 0);

  std::println("Total sum from 1 to {} is: {}", n, total_sum);

  return 0;
}
