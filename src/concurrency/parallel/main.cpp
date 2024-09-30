#include <oneapi/tbb/blocked_range.h>
#include <oneapi/tbb/parallel_reduce.h>

#include <concepts>
#include <functional>
#include <print>

template <typename T>
concept Integral = std::integral<T>;

constexpr auto sum_range = [](Integral auto start,
                              Integral auto end) -> Integral auto {
  return (end - start) * (start + end - 1) / 2;
};

int main() {
  constexpr int n = 1000;         // Range to sum up to
  constexpr int num_threads = 4;  // Number of threads

  // Calculate the range each thread will handle
  constexpr auto range_per_thread = n / num_threads;

  const int total_sum = tbb::parallel_reduce(
      tbb::blocked_range<int>(0, num_threads), 0,
      [&](const tbb::blocked_range<int>& r, int local_sum) {
        for (int i = r.begin(); i < r.end(); ++i) {
          auto start = i * range_per_thread + 1;
          auto end =
              (i == num_threads - 1) ? (n + 1) : (start + range_per_thread);
          local_sum += sum_range(start, end);
        }
        return local_sum;
      },
      std::plus<>());

  std::println("Total sum from 1 to {} is: {}", n, total_sum);

  return 0;
}
