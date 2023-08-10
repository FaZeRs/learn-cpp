#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

enum class RACE { FEDERATION, KLINGON, ANDORIAN, KELPIEN };

constexpr const char *raceToString(RACE race) {
  switch (race) {
    case RACE::FEDERATION:
      return "Federation";
    case RACE::KLINGON:
      return "Klingon";
    case RACE::ANDORIAN:
      return "Andorian";
    case RACE::KELPIEN:
      return "Kelpien";
    default:
      throw "Unknown";
  }
}

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

class KlingonShip : public SpaceShip {
 public:
  [[nodiscard]] std::unique_ptr<SpaceShip> clone() const override {
    return std::make_unique<KlingonShip>(*this);
  }

  void print() const override {
    std::cout << "This is KlingonShip" << std::endl;
  }
};

class StarFleetShip : public SpaceShip {
 public:
  [[nodiscard]] std::unique_ptr<SpaceShip> clone() const override {
    return std::make_unique<StarFleetShip>(*this);
  }

  void print() const override {
    std::cout << "This is StarFleetShip" << std::endl;
  }
};

class AndorianShip : public SpaceShip {
 public:
  [[nodiscard]] std::unique_ptr<SpaceShip> clone() const override {
    return std::make_unique<AndorianShip>(*this);
  }

  void print() const override {
    std::cout << "This is AndorianShip" << std::endl;
  }
};

class SpaceShipFactory {
 public:
  void init() {
    m_space_ships[RACE::KLINGON] = std::make_unique<KlingonShip>();
    m_space_ships[RACE::FEDERATION] = std::make_unique<StarFleetShip>();
    m_space_ships[RACE::ANDORIAN] = std::make_unique<AndorianShip>();
  }

  std::unique_ptr<SpaceShip> createSpaceShip(const RACE &race) {
    if (m_space_ships.find(race) == m_space_ships.end()) {
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
  SpaceShipFactory factory;
  factory.init();

  auto federation_ship = factory.createSpaceShip(RACE::FEDERATION);
  auto klingon_ship = factory.createSpaceShip(RACE::KLINGON);
  auto andorian_ship = factory.createSpaceShip(RACE::ANDORIAN);
  auto kelpein_ship = factory.createSpaceShip(RACE::KELPIEN);

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
