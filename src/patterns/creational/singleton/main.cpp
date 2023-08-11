#include <iostream>
#include <mutex>
#include <thread>

class Logger {
 public:
  Logger(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger& operator=(Logger&&) = delete;
  static Logger& getInstance() {
    static Logger instance;
    std::cout << "Object Address: " << &instance << std::endl;
    return instance;
  }

  void log(const std::string& str) {
    std::scoped_lock<std::mutex> lock(m_mutex);
    std::cout << "Log value: " << str << std::endl;
  }

 private:
  Logger() = default;
  ~Logger() = default;

  std::mutex m_mutex;
};

void threadOne() {
  // Thread One
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  Logger::getInstance().log("Thread ONE is executing");
}

void threadTwo() {
  // Thread Two
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  Logger::getInstance().log("Thread TWO is executing");
}

int main() {
  std::jthread t1(threadOne);
  std::jthread t2(threadTwo);
  Logger::getInstance().log("Parent process is Waiting for threads");
  t1.join();
  t2.join();
  Logger::getInstance().log("Parent process saying BYE BYE");

  return EXIT_SUCCESS;
}