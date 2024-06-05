#include <cstdlib>
#include <iostream>
#include <memory>

// Strategy
class DiscountStrategy {
 public:
  DiscountStrategy() = default;
  virtual ~DiscountStrategy() = default;
  DiscountStrategy(const DiscountStrategy&) = default;
  DiscountStrategy& operator=(const DiscountStrategy&) = default;
  DiscountStrategy(DiscountStrategy&&) = default;
  DiscountStrategy& operator=(DiscountStrategy&&) = default;

  virtual double applyDiscount(double price) = 0;
};

// ConcreteStrategy
class NoDiscount : public DiscountStrategy {
 public:
  double applyDiscount(double price) override {
    return price;  // No discount
  }
};

// ConcreteStrategy
class SeasonalDiscount : public DiscountStrategy {
 public:
  double applyDiscount(double price) override {
    return price * 0.90;  // 10% discount
  }
};

// ConcreteStrategy
class FlatDiscount : public DiscountStrategy {
 public:
  double applyDiscount(double price) override {
    return price - 20;  // Subtract a flat amount
  }
};

// Context
class ShoppingCart {
 public:
  explicit ShoppingCart(DiscountStrategy* strategy) : m_strategy(strategy) {}

  double checkout(double price) { return m_strategy->applyDiscount(price); }

 private:
  std::unique_ptr<DiscountStrategy> m_strategy;
};

int main() {
  ShoppingCart cart_with_no_discount(new NoDiscount());
  std::cout << "Total price with no discount: "
            << cart_with_no_discount.checkout(100) << "\n";

  ShoppingCart cart_with_seasonal_discount(new SeasonalDiscount());
  std::cout << "Total price with seasonal discount: "
            << cart_with_seasonal_discount.checkout(100) << "\n";

  ShoppingCart cart_with_flat_discount(new FlatDiscount());
  std::cout << "Total price with flat discount: "
            << cart_with_flat_discount.checkout(100) << "\n";

  return EXIT_SUCCESS;
}
