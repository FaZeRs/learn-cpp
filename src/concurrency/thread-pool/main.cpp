#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <print>
#include <queue>
#include <stop_token>
#include <thread>
#include <vector>

class ThreadPool {
 public:
  explicit ThreadPool(
      size_t num_threads = std::thread::hardware_concurrency()) {
    workers.reserve(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
      workers.emplace_back([this](std::stop_token st) {
        while (!st.stop_requested()) {
          std::function<void()> task;
          {
            std::unique_lock lock(queue_mutex);
            condition.wait(lock, [this, &st] {
              return !tasks.empty() || st.stop_requested();
            });

            if (st.stop_requested() && tasks.empty()) {
              return;
            }

            task = std::move(tasks.front());
            tasks.pop();
          }
          task();
        }
      });
    }
  }

  ~ThreadPool() {
    for (auto& worker : workers) {
      worker.request_stop();
    }
    condition.notify_all();
  }

  template <typename F, typename... Args>
    requires std::invocable<F, Args...>
  [[nodiscard]] auto enqueue(F&& f, Args&&... args) {
    using return_type = std::invoke_result_t<F, Args...>;
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind_front(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> result = task->get_future();
    {
      std::lock_guard lock(queue_mutex);
      tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return result;
  }

 private:
  std::vector<std::jthread> workers;
  std::queue<std::function<void()>> tasks;
  std::mutex queue_mutex;
  std::condition_variable_any condition;
};

int main() {
  using namespace std::chrono_literals;

  ThreadPool pool(4);  // Create a thread pool with 4 threads

  // Example usage
  auto future1 = pool.enqueue(
      [](int x) {
        std::this_thread::sleep_for(1s);
        return x * x;
      },
      23);

  auto future2 = pool.enqueue(
      [](int x, int y) {
        std::this_thread::sleep_for(1s);
        return x + y;
      },
      2, 3);

  std::println("Result 1: {}", future1.get());  // 529
  std::println("Result 2: {}", future2.get());  // 5

  return 0;
}
