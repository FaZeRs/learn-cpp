#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

// Step 1: Define the interface for the core functionality
class Component {
 public:
  Component() = default;
  virtual ~Component() = default;
  Component(const Component&) = default;
  Component& operator=(const Component&) = default;
  Component(Component&&) = default;
  Component& operator=(Component&&) = default;
  [[nodiscard]] virtual std::string Operation() const = 0;
};

// Step 2: Create concrete components
class ConcreteComponent : public Component {
 public:
  [[nodiscard]] std::string Operation() const override {
    return "ConcreteComponent";
  }
};

// Step 3: Create the base decorator
class Decorator : public Component {
 public:
  explicit Decorator(Component* comp) : m_component(comp) {}
  [[nodiscard]] std::string Operation() const override {
    return m_component->Operation();
  }

 private:
  Component* m_component;
};

// Step 4: Create concrete decorators
class ConcreteDecoratorA : public Decorator {
  using Decorator::Decorator;

 public:
  [[nodiscard]] std::string Operation() const override {
    return "ConcreteDecoratorA(" + Decorator::Operation() + ")";
  }
};

class ConcreteDecoratorB : public Decorator {
  using Decorator::Decorator;

 public:
  [[nodiscard]] std::string Operation() const override {
    return "ConcreteDecoratorB(" + Decorator::Operation() + ")";
  }
};

// Client code
int main() {
  std::unique_ptr<Component> simple = std::make_unique<ConcreteComponent>();
  std::cout << "Simple component: " << simple->Operation() << std::endl;

  std::unique_ptr<Component> decoratorA =
      std::make_unique<ConcreteDecoratorA>(simple.get());
  std::cout << "A decorated: " << decoratorA->Operation() << std::endl;

  std::unique_ptr<Component> decoratorB =
      std::make_unique<ConcreteDecoratorB>(decoratorA.get());
  std::cout << "B decorated A: " << decoratorB->Operation() << std::endl;

  return EXIT_SUCCESS;
}