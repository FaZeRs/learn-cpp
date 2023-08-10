#include <iostream>
#include <memory>

// Receiver
class Light {
 public:
  void on() { m_is_on = true; }
  void off() { m_is_on = false; }
  void print() const {
    std::cout << "Light is " << (m_is_on ? "ON" : "OFF") << std::endl;
  }

 private:
  bool m_is_on{false};
};

// Command Interface
class Command {
 public:
  Command() = default;
  virtual ~Command() = default;
  Command(const Command &) = default;
  Command &operator=(const Command &) = default;
  Command(Command &&) = default;
  Command &operator=(Command &&) = default;
  virtual void execute() = 0;
};

// Concrete Command
class LightOnCommand : public Command {
 public:
  explicit LightOnCommand(const std::shared_ptr<Light> &l) : m_light(l) {}
  void execute() override { m_light->on(); }

 private:
  std::shared_ptr<Light> m_light;
};

// Concrete Command
class LightOffCommand : public Command {
 public:
  explicit LightOffCommand(const std::shared_ptr<Light> &l) : m_light(l) {}
  void execute() override { m_light->off(); }

 private:
  std::shared_ptr<Light> m_light;
};

// Invoker
class RemoteControl {
  std::unique_ptr<Command> command;

 public:
  void setCommand(std::unique_ptr<Command> c) { command = std::move(c); }

  void pressButton() { command->execute(); }
};

int main() {
  auto light = std::make_shared<Light>();
  RemoteControl remote;

  remote.setCommand(std::make_unique<LightOnCommand>(light));
  remote.pressButton();
  light->print();

  remote.setCommand(std::make_unique<LightOffCommand>(light));
  remote.pressButton();
  light->print();

  return 0;
}