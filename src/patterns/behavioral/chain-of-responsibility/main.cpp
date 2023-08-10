#include <iostream>
#include <memory>
#include <vector>

class Handler {
 public:
  Handler() = default;
  virtual ~Handler() = default;
  Handler(const Handler &) = default;
  Handler &operator=(const Handler &) = default;
  Handler(Handler &&) = default;
  Handler &operator=(Handler &&) = default;

  virtual Handler *SetNext(Handler *handler) = 0;
  virtual std::string Handle(const std::string &request) = 0;
};

class AbstractHandler : public Handler {
 public:
  Handler *SetNext(Handler *handler) override {
    m_next_handler = handler;
    return handler;
  }
  std::string Handle(const std::string &request) override {
    if (m_next_handler) {
      return m_next_handler->Handle(request);
    }

    return {};
  }

 private:
  Handler *m_next_handler{nullptr};
};

class MonkeyHandler : public AbstractHandler {
 public:
  std::string Handle(const std::string &request) override {
    if (request == "Banana") {
      return "Monkey: I'll eat the " + request + ".";
    } else {
      return AbstractHandler::Handle(request);
    }
  }
};

class SquirrelHandler : public AbstractHandler {
 public:
  std::string Handle(const std::string &request) override {
    if (request == "Nut") {
      return "Squirrel: I'll eat the " + request + ".";
    } else {
      return AbstractHandler::Handle(request);
    }
  }
};

class DogHandler : public AbstractHandler {
 public:
  std::string Handle(const std::string &request) override {
    if (request == "MeatBall") {
      return "Dog: I'll eat the " + request + ".";
    } else {
      return AbstractHandler::Handle(request);
    }
  }
};

void ClientCode(Handler &handler) {
  std::vector<std::string> food = {"Nut", "Banana", "Cup of coffee"};
  for (const std::string &f : food) {
    std::cout << "Client: Who wants a " << f << "?" << std::endl;
    const std::string result = handler.Handle(f);
    if (!result.empty()) {
      std::cout << "  " << result << std::endl;
    } else {
      std::cout << "  " << f << " was left untouched." << std::endl;
    }
  }
}

int main() {
  auto monkey = std::make_unique<MonkeyHandler>();
  auto squirrel = std::make_unique<SquirrelHandler>();
  auto dog = std::make_unique<DogHandler>();
  monkey->SetNext(squirrel.get())->SetNext(dog.get());

  std::cout << "Chain: Monkey > Squirrel > Dog" << std::endl << std::endl;
  ClientCode(*monkey);
  std::cout << std::endl;
  std::cout << "Subchain: Squirrel > Dog" << std::endl << std::endl;
  ClientCode(*squirrel);

  return EXIT_SUCCESS;
}