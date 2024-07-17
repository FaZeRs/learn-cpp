#include <mutex>
#include <print>
#include <thread>

int main() {
  std::mutex mtx;
  int number = 0;

  auto increment = [&mtx, &number]() {
    std::lock_guard lock(mtx);
    for (int i = 0; i < 1000000; i++) {
      number++;
    }
  };

  std::jthread t1(increment);
  std::jthread t2(increment);
  t1.join();
  t2.join();
  std::println("Number after execution of t1 and t2 is {}", number);
  return 0;
}