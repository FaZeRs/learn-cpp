#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

class Flyweight {
 public:
  Flyweight() = default;
  virtual ~Flyweight() = default;
  Flyweight(const Flyweight &) = default;
  Flyweight &operator=(const Flyweight &) = default;
  Flyweight(Flyweight &&) = default;
  Flyweight &operator=(Flyweight &&) = default;
  virtual void operation(const std::string &extrinsic_state) = 0;
};

class ConcreteFlyweight : public Flyweight {
 public:
  explicit ConcreteFlyweight(std::string state)
      : m_intrinsic_state(std::move(state)) {}

  void operation(const std::string &extrinsic_state) override {
    std::cout << "ConcreteFlyweight with intrinsicState = " << m_intrinsic_state
              << " operating with extrinsicState = " << extrinsic_state
              << std::endl;
  }

 private:
  std::string m_intrinsic_state;
};

class FlyweightFactory {
 public:
  std::shared_ptr<Flyweight> getFlyweight(const std::string &key) {
    if (m_flyweights.find(key) == m_flyweights.end()) {
      m_flyweights.try_emplace(key, std::make_shared<ConcreteFlyweight>(key));
    }
    return m_flyweights[key];
  }

  size_t getCount() const { return m_flyweights.size(); }

 private:
  std::unordered_map<std::string, std::shared_ptr<Flyweight>> m_flyweights;
};

int main() {
  FlyweightFactory factory;

  auto flyweightA = factory.getFlyweight("A");
  auto flyweightB = factory.getFlyweight("B");
  auto flyweightA2 = factory.getFlyweight("A");

  flyweightA->operation("test1");
  flyweightB->operation("test2");
  flyweightA2->operation("test3");

  std::cout << "Number of flyweights: " << factory.getCount() << std::endl;

  return EXIT_SUCCESS;
}