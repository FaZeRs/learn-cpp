#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

// Abstract State class
class LightState {
 public:
  LightState() = default;
  virtual ~LightState() = default;
  LightState(const LightState&) = default;
  LightState& operator=(const LightState&) = default;
  LightState(LightState&&) = default;
  LightState& operator=(LightState&&) = default;

  virtual void turnOn() = 0;
  virtual void turnOff() = 0;
};

// Concrete State classes
class OnState : public LightState {
 public:
  void turnOn() override { std::cout << "Light is already On!" << std::endl; }

  void turnOff() override {
    std::cout << "Turning off the light." << std::endl;
  }
};

class OffState : public LightState {
 public:
  void turnOn() override { std::cout << "Turning on the light." << std::endl; }

  void turnOff() override { std::cout << "Light is already Off!" << std::endl; }
};

// Context class
class LightSwitch {
 public:
  LightSwitch() : m_state(new OffState) {}

  void setState(std::unique_ptr<LightState> newState) {
    m_state = std::move(newState);
  }

  void turnOn() {
    m_state->turnOn();
    setState(std::make_unique<OnState>());
  }

  void turnOff() {
    m_state->turnOff();
    setState(std::make_unique<OffState>());
  }

 private:
  std::unique_ptr<LightState> m_state;
};

int main() {
  LightSwitch light;

  light.turnOn();   // Output: Turning on the light.
  light.turnOn();   // Output: Light is already On!
  light.turnOff();  // Output: Turning off the light.
  light.turnOff();  // Output: Light is already Off!

  return EXIT_SUCCESS;
}