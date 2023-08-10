#include <iostream>
#include <memory>

class Implementation {
 public:
  Implementation() = default;
  virtual ~Implementation() = default;
  Implementation(const Implementation&) = default;
  Implementation& operator=(const Implementation&) = default;
  Implementation(Implementation&&) = default;
  Implementation& operator=(Implementation&&) = default;

  virtual void operationImpl() = 0;
};

class ConcreteImplementationA : public Implementation {
 public:
  void operationImpl() override {
    std::cout << "Concrete Implementation A" << std::endl;
  }
};

class ConcreteImplementationB : public Implementation {
 public:
  void operationImpl() override {
    std::cout << "Concrete Implementation B" << std::endl;
  }
};

class Abstraction {
 public:
  explicit Abstraction(Implementation* impl) : impl(impl) {}
  virtual ~Abstraction() = default;
  Abstraction(const Abstraction&) = default;
  Abstraction& operator=(const Abstraction&) = default;
  Abstraction(Abstraction&&) = default;
  Abstraction& operator=(Abstraction&&) = default;
  virtual void operation() = 0;

 protected:
  class Implementation* impl;  // NOLINT
};

class RefinedAbstraction : public Abstraction {
 public:
  explicit RefinedAbstraction(Implementation* impl) : Abstraction(impl) {}
  void operation() override {
    std::cout << "Refined Abstraction: ";
    impl->operationImpl();
  }
};

int main() {
  std::unique_ptr<Implementation> concrete_impl_a =
      std::make_unique<ConcreteImplementationA>();
  std::unique_ptr<Implementation> concrete_impl_b =
      std::make_unique<ConcreteImplementationB>();

  std::unique_ptr<Abstraction> abstraction_1 =
      std::make_unique<RefinedAbstraction>(concrete_impl_a.get());
  std::unique_ptr<Abstraction> abstraction_2 =
      std::make_unique<RefinedAbstraction>(concrete_impl_b.get());

  abstraction_1
      ->operation();  // Outputs: Refined Abstraction: Concrete Implementation A
  abstraction_2
      ->operation();  // Outputs: Refined Abstraction: Concrete Implementation B

  return EXIT_SUCCESS;
}