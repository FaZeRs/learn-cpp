#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <unordered_map>

enum class RACE : uint8_t { FEDERATION, KLINGON, ANDORIAN, KELPIEN };

constexpr const char *raceToString(RACE race) {
  switch (race) {
    using enum RACE;
    case FEDERATION:
      return "Federation";
    case KLINGON:
      return "Klingon";
    case ANDORIAN:
      return "Andorian";
    case KELPIEN:
      return "Kelpien";
    default:
      return "Unknown";
  }
}

// Prototype
class SpaceShip {
 public:
  SpaceShip() = default;
  virtual ~SpaceShip() = default;
  SpaceShip(const SpaceShip &) = default;
  SpaceShip &operator=(const SpaceShip &) = default;
  SpaceShip(SpaceShip &&) = default;
  SpaceShip &operator=(SpaceShip &&) = default;

  [[nodiscard]] virtual std::unique_ptr<SpaceShip> clone() const = 0;
  virtual void print() const = 0;
};

// ConcretePrototype
class KlingonShip : public SpaceShip {
 public:
  [[nodiscard]] std::unique_ptr<SpaceShip> clone() const override {
    return std::make_unique<KlingonShip>(*this);
  }

  void print() const override { std::cout << "This is KlingonShip\n"; }
};

// ConcretePrototype
class StarFleetShip : public SpaceShip {
 public:
  [[nodiscard]] std::unique_ptr<SpaceShip> clone() const override {
    return std::make_unique<StarFleetShip>(*this);
  }

  void print() const override { std::cout << "This is StarFleetShip\n"; }
};

// ConcretePrototype
class AndorianShip : public SpaceShip {
 public:
  [[nodiscard]] std::unique_ptr<SpaceShip> clone() const override {
    return std::make_unique<AndorianShip>(*this);
  }

  void print() const override { std::cout << "This is AndorianShip\n"; }
};

// Client
class SpaceShipFactory {
 public:
  void init() {
    using enum RACE;
    m_space_ships[KLINGON] = std::make_unique<KlingonShip>();
    m_space_ships[FEDERATION] = std::make_unique<StarFleetShip>();
    m_space_ships[ANDORIAN] = std::make_unique<AndorianShip>();
  }

  std::unique_ptr<SpaceShip> createSpaceShip(const RACE &race) {
    if (!m_space_ships.contains(race)) {
      std::cout << "SpaceShip not found for race: " << raceToString(race)
                << "\n";
      return nullptr;
    }
    return m_space_ships[race]->clone();
  }

 private:
  std::unordered_map<RACE, std::unique_ptr<SpaceShip>> m_space_ships{};
};

int main() {
  using enum RACE;

  SpaceShipFactory factory;
  factory.init();

  auto federation_ship = factory.createSpaceShip(FEDERATION);
  auto klingon_ship = factory.createSpaceShip(KLINGON);
  auto andorian_ship = factory.createSpaceShip(ANDORIAN);
  auto kelpein_ship = factory.createSpaceShip(KELPIEN);

  if (federation_ship) {
    federation_ship->print();
  }
  if (klingon_ship) {
    klingon_ship->print();
  }
  if (andorian_ship) {
    andorian_ship->print();
  }
  if (kelpein_ship) {
    kelpein_ship->print();
  }

  return EXIT_SUCCESS;
}
