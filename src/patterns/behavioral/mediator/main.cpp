#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

// Mediator interface
class Mediator;

class Colleague {
 public:
  explicit Colleague(Mediator* m) : m_mediator(m) {}
  virtual ~Colleague() = default;
  Colleague(const Colleague&) = default;
  Colleague& operator=(const Colleague&) = default;
  Colleague(Colleague&&) = default;
  Colleague& operator=(Colleague&&) = default;

  virtual void send(const std::string& message) = 0;
  virtual void receive(const std::string& message) = 0;
  [[nodiscard]] Mediator* mediator() const { return m_mediator; }

 private:
  Mediator* m_mediator;
};

class Mediator {
 public:
  Mediator() = default;
  virtual ~Mediator() = default;
  Mediator(const Mediator&) = default;
  Mediator& operator=(const Mediator&) = default;
  Mediator(Mediator&&) = default;
  Mediator& operator=(Mediator&&) = default;

  [[nodiscard]] inline const std::list<Colleague*>& colleagues() const {
    return m_colleagues;
  }
  virtual void addColleague(Colleague* colleague) {
    m_colleagues.emplace_back(colleague);
  }

  virtual void distributeMessage(Colleague* sender,
                                 const std::string& message) = 0;

 private:
  std::list<Colleague*> m_colleagues;
};

class ChatRoom : public Mediator {
 public:
  void distributeMessage(Colleague* sender,
                         const std::string& message) override {
    for (auto colleague : colleagues()) {
      if (colleague != sender) {
        colleague->receive(message);
      }
    }
  }
};

class User : public Colleague {
 public:
  User(Mediator* m, std::string name) : Colleague(m), m_name(std::move(name)) {
    m->addColleague(this);
  }

  void send(const std::string& message) override {
    std::cout << m_name << " sends: " << message << std::endl;
    mediator()->distributeMessage(this, message);
  }

  void receive(const std::string& message) override {
    std::cout << m_name << " received: " << message << std::endl;
  }

 private:
  std::string m_name;
};

int main() {
  ChatRoom chat_room;

  User alice(&chat_room, "Alice");
  User bob(&chat_room, "Bob");
  User carol(&chat_room, "Carol");

  alice.send("Hi everyone!");
  bob.send("Hey Alice, how are you?");
  carol.send("Hello!");

  return EXIT_SUCCESS;
}