#include <iostream>

// Abstract base class defining template method and some abstract operations
class AbstractClass {
 public:
  AbstractClass() = default;
  virtual ~AbstractClass() = default;
  AbstractClass(const AbstractClass&) = default;
  AbstractClass& operator=(const AbstractClass&) = default;
  AbstractClass(AbstractClass&&) = default;
  AbstractClass& operator=(AbstractClass&&) = default;

  // Template method
  void templateMethod() const {
    baseOperation1();
    requiredOperations1();
    baseOperation2();
    hook();
  }

  // Concrete operations with default implementations
  static void baseOperation1() { std::cout << "Base operation 1" << std::endl; }

  static void baseOperation2() { std::cout << "Base operation 2" << std::endl; }

  // Abstract operations that must be implemented by concrete subclasses
  virtual void requiredOperations1() const = 0;

  // Hook operation with default empty implementation
  virtual void hook() const {}
};

class ConcreteClass1 : public AbstractClass {
 protected:
  void requiredOperations1() const override {
    std::cout << "Concrete class 1 implementation of required operation 1"
              << std::endl;
  }

  void hook() const override {
    std::cout << "Concrete class 1 hook operation" << std::endl;
  }
};

class ConcreteClass2 : public AbstractClass {
 protected:
  void requiredOperations1() const override {
    std::cout << "Concrete class 2 implementation of required operation 1"
              << std::endl;
  }
};

int main() {
  std::cout << "Template method in ConcreteClass1:" << std::endl;
  ConcreteClass1 concreteClass1;
  concreteClass1.templateMethod();

  std::cout << std::endl << "Template method in ConcreteClass2:" << std::endl;
  ConcreteClass2 concreteClass2;
  concreteClass2.templateMethod();

  return 0;
}