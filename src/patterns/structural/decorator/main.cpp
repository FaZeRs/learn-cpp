#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

// Component
class Beverage {
 public:
  Beverage() = default;
  virtual ~Beverage() = default;
  Beverage(const Beverage&) = default;
  Beverage& operator=(const Beverage&) = default;
  Beverage(Beverage&&) = default;
  Beverage& operator=(Beverage&&) = default;

  [[nodiscard]] virtual double cost() const = 0;
  [[nodiscard]] virtual std::string description() const = 0;
};

// ConcreteComponent
class Coffee : public Beverage {
 public:
  [[nodiscard]] double cost() const override { return 1.0; }
  [[nodiscard]] std::string description() const override { return "Coffee"; }
};

// Decorator
class BeverageDecorator : public Beverage {
 public:
  explicit BeverageDecorator(std::unique_ptr<Beverage> b)
      : m_beverage(std::move(b)) {}

  [[nodiscard]] Beverage* beverage() const { return m_beverage.get(); }

 private:
  std::unique_ptr<Beverage> m_beverage;
};

// ConcreteDecorator
class Milk : public BeverageDecorator {
 public:
  explicit Milk(std::unique_ptr<Beverage> b)
      : BeverageDecorator(std::move(b)) {}
  [[nodiscard]] double cost() const override {
    return 0.2 + beverage()->cost();
  }
  [[nodiscard]] std::string description() const override {
    return beverage()->description() + ", Milk";
  }
};

// ConcreteDecorator
class Sugar : public BeverageDecorator {
 public:
  explicit Sugar(std::unique_ptr<Beverage> b)
      : BeverageDecorator(std::move(b)) {}
  [[nodiscard]] double cost() const override {
    return 0.1 + beverage()->cost();
  }
  [[nodiscard]] std::string description() const override {
    return beverage()->description() + ", Sugar";
  }
};

int main() {
  std::unique_ptr<Beverage> coffee = std::make_unique<Coffee>();
  std::cout << coffee->description() << " - $" << coffee->cost() << std::endl;

  coffee = std::make_unique<Milk>(std::move(coffee));
  std::cout << coffee->description() << " - $" << coffee->cost() << std::endl;

  coffee = std::make_unique<Sugar>(std::move(coffee));
  std::cout << coffee->description() << " - $" << coffee->cost() << std::endl;

  return EXIT_SUCCESS;
}
