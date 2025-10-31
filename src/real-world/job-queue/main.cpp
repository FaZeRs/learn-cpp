#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <print>
#include <queue>
#include <stop_token>
#include <thread>
#include <vector>

// Forward declarations
class JobQueue;

// Priority levels for tasks
enum class Priority : std::uint8_t { Low = 0, Medium = 1, High = 2 };

// Task handle for cancellation
class TaskHandle {
 public:
  TaskHandle() : cancelled_(std::make_shared<std::atomic<bool>>(false)) {}

  void cancel() { cancelled_->store(true, std::memory_order_release); }
  [[nodiscard]] bool is_cancelled() const {
    return cancelled_->load(std::memory_order_acquire);
  }

 private:
  friend class JobQueue;
  std::shared_ptr<std::atomic<bool>> cancelled_;
};

// Internal task representation
struct Task {
  using clock_type = std::chrono::steady_clock;
  using time_point = std::chrono::steady_clock::time_point;
  using duration = std::chrono::steady_clock::duration;

  std::function<void()> func;
  Priority priority;
  time_point scheduled_time;
  duration recurring_interval;
  bool is_recurring;
  std::shared_ptr<std::atomic<bool>> cancelled;

  Task(std::function<void()> f, Priority p, time_point st, duration ri,
       bool recurring, std::shared_ptr<std::atomic<bool>> c)
      : func(std::move(f)),
        priority(p),
        scheduled_time(st),
        recurring_interval(ri),
        is_recurring(recurring),
        cancelled(std::move(c)) {}

  [[nodiscard]] bool is_cancelled() const {
    return cancelled && cancelled->load(std::memory_order_acquire);
  }

  [[nodiscard]] bool is_ready() const {
    return clock_type::now() >= scheduled_time;
  }
};

// Comparator for priority queue (higher priority and earlier time first)
struct TaskComparator {
  bool operator()(const std::shared_ptr<Task>& a,
                  const std::shared_ptr<Task>& b) const {
    if (a->priority != b->priority) {
      return a->priority < b->priority;  // Higher priority first
    }
    return a->scheduled_time > b->scheduled_time;  // Earlier time first
  }
};

// Simple thread pool
class ThreadPool {
 public:
  explicit ThreadPool(
      size_t num_threads = std::thread::hardware_concurrency()) {
    workers.reserve(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
      workers.emplace_back([this](const std::stop_token& st) {
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

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  void enqueue(std::function<void()> task) {
    {
      std::lock_guard lock(queue_mutex);
      tasks.emplace(std::move(task));
    }
    condition.notify_one();
  }

  [[nodiscard]] size_t pending_tasks() const {
    std::lock_guard lock(queue_mutex);
    return tasks.size();
  }

 private:
  std::vector<std::jthread> workers;
  std::queue<std::function<void()>> tasks;
  mutable std::mutex queue_mutex;
  std::condition_variable_any condition;
};

// Job Queue with scheduling capabilities
class JobQueue {
 public:
  explicit JobQueue(size_t num_threads = std::thread::hardware_concurrency())
      : thread_pool_(num_threads),
        scheduler_thread_(
            [this](const std::stop_token& st) { scheduler_loop(st); }) {}

  ~JobQueue() {
    scheduler_thread_.request_stop();
    scheduler_cv_.notify_all();
  }

  JobQueue(const JobQueue&) = delete;
  JobQueue& operator=(const JobQueue&) = delete;
  JobQueue(JobQueue&&) = delete;
  JobQueue& operator=(JobQueue&&) = delete;

  // Submit immediate task
  template <typename F, typename... Args>
    requires std::invocable<F, Args...>
  TaskHandle submit(F&& func, Args&&... args,
                    Priority priority = Priority::Medium) {
    TaskHandle handle;
    auto task = std::make_shared<Task>(
        [f = std::forward<F>(func),
         ... a = std::forward<Args>(args)]() mutable {
          std::invoke(std::forward<F>(f), std::forward<Args>(a)...);
        },
        priority, Task::clock_type::now(), Task::duration::zero(), false,
        handle.cancelled_);

    add_to_ready_queue(task);
    return handle;
  }

  // Submit delayed task
  template <typename F, typename... Args>
    requires std::invocable<F, Args...>
  TaskHandle submit_delayed(std::chrono::milliseconds delay, F&& func,
                            Args&&... args,
                            Priority priority = Priority::Medium) {
    TaskHandle handle;
    auto task = std::make_shared<Task>(
        [f = std::forward<F>(func),
         ... a = std::forward<Args>(args)]() mutable {
          std::invoke(std::forward<F>(f), std::forward<Args>(a)...);
        },
        priority, Task::clock_type::now() + delay, Task::duration::zero(),
        false, handle.cancelled_);

    add_to_scheduled_queue(task);
    return handle;
  }

  // Submit recurring task
  template <typename F, typename... Args>
    requires std::invocable<F, Args...>
  TaskHandle submit_recurring(std::chrono::milliseconds interval, F&& func,
                              Args&&... args,
                              Priority priority = Priority::Medium) {
    TaskHandle handle;
    auto task = std::make_shared<Task>(
        [f = std::forward<F>(func),
         ... a = std::forward<Args>(args)]() mutable {
          std::invoke(std::forward<F>(f), std::forward<Args>(a)...);
        },
        priority, Task::clock_type::now() + interval, interval, true,
        handle.cancelled_);

    add_to_scheduled_queue(task);
    return handle;
  }

  // Get statistics
  struct Stats {
    size_t ready_tasks;
    size_t scheduled_tasks;
    size_t executing_tasks;
  };

  [[nodiscard]] Stats get_stats() const {
    Stats stats{};
    {
      std::lock_guard lock(ready_mutex_);
      stats.ready_tasks = ready_queue_.size();
    }
    {
      std::lock_guard lock(scheduled_mutex_);
      stats.scheduled_tasks = scheduled_queue_.size();
    }
    stats.executing_tasks = thread_pool_.pending_tasks();
    return stats;
  }

 private:
  void add_to_ready_queue(const std::shared_ptr<Task>& task) {
    {
      std::lock_guard lock(ready_mutex_);
      ready_queue_.push(task);
    }
    process_ready_queue();
  }

  void add_to_scheduled_queue(const std::shared_ptr<Task>& task) {
    {
      std::lock_guard lock(scheduled_mutex_);
      scheduled_queue_.push(task);
    }
    scheduler_cv_.notify_one();
  }

  void process_ready_queue() {
    std::lock_guard lock(ready_mutex_);
    while (!ready_queue_.empty()) {
      auto task = ready_queue_.top();
      ready_queue_.pop();

      if (task->is_cancelled()) {
        continue;
      }

      // Wrap task to handle recurring tasks
      if (task->is_recurring) {
        thread_pool_.enqueue([this, task]() {
          if (!task->is_cancelled()) {
            task->func();

            // Reschedule if not cancelled
            if (!task->is_cancelled()) {
              auto next_task = std::make_shared<Task>(
                  task->func, task->priority,
                  Task::clock_type::now() + task->recurring_interval,
                  task->recurring_interval, true, task->cancelled);
              add_to_scheduled_queue(next_task);
            }
          }
        });
      } else {
        thread_pool_.enqueue([task]() {
          if (!task->is_cancelled()) {
            task->func();
          }
        });
      }
    }
  }

  void scheduler_loop(const std::stop_token& st) {
    while (!st.stop_requested()) {
      std::unique_lock lock(scheduled_mutex_);

      // Calculate wait time
      auto wait_duration = std::chrono::milliseconds(100);
      if (!scheduled_queue_.empty()) {
        auto next_time = scheduled_queue_.top()->scheduled_time;
        auto now = Task::clock_type::now();
        if (next_time > now) {
          wait_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
              next_time - now);
        } else {
          wait_duration = std::chrono::milliseconds(0);
        }
      }

      scheduler_cv_.wait_for(lock, wait_duration,
                             [&st] { return st.stop_requested(); });

      if (st.stop_requested()) {
        break;
      }

      // Move ready tasks to ready queue
      std::vector<std::shared_ptr<Task>> ready_tasks;
      while (!scheduled_queue_.empty() && scheduled_queue_.top()->is_ready()) {
        auto task = scheduled_queue_.top();
        scheduled_queue_.pop();

        if (!task->is_cancelled()) {
          ready_tasks.push_back(task);
        }
      }

      lock.unlock();

      // Add ready tasks to ready queue
      for (const auto& task : ready_tasks) {
        add_to_ready_queue(task);
      }
    }
  }

  ThreadPool thread_pool_;
  std::jthread scheduler_thread_;

  std::priority_queue<std::shared_ptr<Task>, std::vector<std::shared_ptr<Task>>,
                      TaskComparator>
      ready_queue_;
  mutable std::mutex ready_mutex_;

  std::priority_queue<std::shared_ptr<Task>, std::vector<std::shared_ptr<Task>>,
                      TaskComparator>
      scheduled_queue_;
  mutable std::mutex scheduled_mutex_;

  std::condition_variable_any scheduler_cv_;
};

// Helper function to print with timestamp
void print_with_time(std::string_view msg) {
  auto now = std::chrono::system_clock::now();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) %
            1000;
  std::println("[{:03}ms] {}", ms.count(), msg);
}

int main() {
  using namespace std::chrono_literals;

  std::println("=== Job Queue / Task Scheduler Demo ===\n");

  JobQueue queue(4);

  // Example 1: Immediate tasks with different priorities
  std::println("1. Submitting immediate tasks with priorities:");
  queue.submit([] { print_with_time("Low priority task"); }, Priority::Low);
  queue.submit([] { print_with_time("High priority task"); }, Priority::High);
  queue.submit([] { print_with_time("Medium priority task"); },
               Priority::Medium);
  queue.submit([] { print_with_time("Another high priority task"); },
               Priority::High);

  std::this_thread::sleep_for(100ms);

  // Example 2: Delayed tasks
  std::println("\n2. Submitting delayed tasks:");
  queue.submit_delayed(500ms, [] { print_with_time("Task delayed by 500ms"); });
  queue.submit_delayed(200ms, [] { print_with_time("Task delayed by 200ms"); });
  queue.submit_delayed(800ms, [] { print_with_time("Task delayed by 800ms"); });

  std::this_thread::sleep_for(1s);

  // Example 3: Recurring tasks
  std::println("\n3. Starting recurring task (every 300ms for 5 times):");
  std::atomic<int> counter{0};
  auto recurring_handle = queue.submit_recurring(300ms, [&counter] {
    int count = counter.fetch_add(1, std::memory_order_relaxed) + 1;
    print_with_time(std::format("Recurring task execution #{}", count));
  });

  std::this_thread::sleep_for(1600ms);

  // Cancel the recurring task
  std::println("\n4. Cancelling recurring task...");
  recurring_handle.cancel();

  std::this_thread::sleep_for(500ms);

  // Example 4: Task cancellation
  std::println("\n5. Testing task cancellation:");
  auto handle1 = queue.submit_delayed(
      300ms, [] { print_with_time("This will be cancelled"); });
  auto handle2 =
      queue.submit_delayed(300ms, [] { print_with_time("This will execute"); });

  // Cancel first task immediately
  handle1.cancel();
  std::println("Cancelled first delayed task");

  std::this_thread::sleep_for(500ms);

  // Example 5: Statistics
  std::println("\n6. Queue statistics:");
  for (int i = 0; i < 20; ++i) {
    queue.submit_delayed(std::chrono::milliseconds(i * 50),
                         [] { std::this_thread::sleep_for(100ms); });
  }

  std::this_thread::sleep_for(100ms);
  auto stats = queue.get_stats();
  std::println("Ready tasks: {}", stats.ready_tasks);
  std::println("Scheduled tasks: {}", stats.scheduled_tasks);
  std::println("Executing tasks: {}", stats.executing_tasks);

  std::this_thread::sleep_for(2s);

  std::println("\n=== Demo Complete ===");
  return 0;
}
