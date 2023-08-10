#include <algorithm>
#include <iostream>
#include <memory>

class Strategy {
 public:
  Strategy() = default;
  virtual ~Strategy() = default;
  Strategy(const Strategy&) = default;
  Strategy& operator=(const Strategy&) = default;
  Strategy(Strategy&&) = default;
  Strategy& operator=(Strategy&&) = default;

  [[nodiscard]] virtual std::string doAlgorithm(
      std::string_view data) const = 0;
};

class Context {
 private:
  std::unique_ptr<Strategy> strategy_;

 public:
  explicit Context(std::unique_ptr<Strategy>&& strategy = {})
      : strategy_(std::move(strategy)) {}
  void set_strategy(std::unique_ptr<Strategy>&& strategy) {
    strategy_ = std::move(strategy);
  }

  void doSomeBusinessLogic() const {
    if (strategy_) {
      std::cout << "Context: Sorting data using the strategy" << std::endl;
      std::string result = strategy_->doAlgorithm("aecbd");
      std::cout << result << std::endl;
    } else {
      std::cout << "Context: Strategy isn't set" << std::endl;
    }
  }
};

class ConcreteStrategyA : public Strategy {
 public:
  [[nodiscard]] std::string doAlgorithm(std::string_view data) const override {
    std::string result(data);
    std::sort(std::begin(result), std::end(result));

    return result;
  }
};
class ConcreteStrategyB : public Strategy {
  [[nodiscard]] std::string doAlgorithm(std::string_view data) const override {
    std::string result(data);
    std::sort(std::begin(result), std::end(result), std::greater<>());

    return result;
  }
};

void clientCode() {
  Context context(std::make_unique<ConcreteStrategyA>());
  std::cout << "Client: Strategy is set to normal sorting." << std::endl;
  context.doSomeBusinessLogic();
  std::cout << std::endl;
  std::cout << "Client: Strategy is set to reverse sorting." << std::endl;
  context.set_strategy(std::make_unique<ConcreteStrategyB>());
  context.doSomeBusinessLogic();
}

int main() {
  clientCode();
  return 0;
}