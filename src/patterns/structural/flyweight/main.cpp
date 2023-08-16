#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

class Soldier {
 public:
  Soldier() = default;
  virtual ~Soldier() = default;
  Soldier(const Soldier &) = default;
  Soldier &operator=(const Soldier &) = default;
  Soldier(Soldier &&) = default;
  Soldier &operator=(Soldier &&) = default;
  virtual void display(int x, int y) = 0;
};

class PrivateSoldier : public Soldier {
 public:
  void display(int x, int y) override {
    std::cout << "Private soldier displayed at (" << x << ", " << y << ")"
              << std::endl;
  }
};

class CorporalSoldier : public Soldier {
 public:
  void display(int x, int y) override {
    std::cout << "Corporal soldier displayed at (" << x << ", " << y << ")"
              << std::endl;
  }
};

class SoldierFactory {
 public:
  std::shared_ptr<Soldier> getSoldier(const std::string &key) {
    if (m_soldiers.find(key) == m_soldiers.end()) {
      if (key == "Private") {
        m_soldiers.try_emplace(key, std::make_shared<PrivateSoldier>());
      } else if (key == "Corporal") {
        m_soldiers.try_emplace(key, std::make_shared<CorporalSoldier>());
      }
    }
    return m_soldiers[key];
  }

  size_t getCount() const { return m_soldiers.size(); }

 private:
  std::unordered_map<std::string, std::shared_ptr<Soldier>> m_soldiers;
};

int main() {
  SoldierFactory factory;

  std::cout << "Number of soldiers: " << factory.getCount() << std::endl;

  auto soldierA = factory.getSoldier("Private");
  auto soldierB = factory.getSoldier("Corporal");
  auto soldierA2 = factory.getSoldier("Private");

  soldierA->display(10, 20);
  soldierB->display(30, 40);
  soldierA2->display(60, 70);

  std::cout << "Number of soldiers: " << factory.getCount() << std::endl;

  return EXIT_SUCCESS;
}