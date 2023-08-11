#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <string_view>
#include <utility>

// Observer abstract class
class Observer {
 public:
  Observer() = default;
  virtual ~Observer() = default;
  Observer(const Observer&) = default;
  Observer& operator=(const Observer&) = default;
  Observer(Observer&&) = default;
  Observer& operator=(Observer&&) = default;

  virtual void update(const std::string& message) = 0;
};

// ConcreteObserver class
class ConcreteObserver : public Observer {
 public:
  explicit ConcreteObserver(std::string name) : m_name(std::move(name)) {}

  void update(const std::string& message) override {
    std::cout << "Observer " << m_name << " received message: " << message
              << std::endl;
  }

 private:
  std::string m_name;
};

// Subject abstract class
class Subject {
 public:
  void addObserver(Observer* observer) { m_observers.emplace_back(observer); }

  void removeObserver(Observer* observer) { m_observers.remove(observer); }

  void notifyObservers(const std::string& message) const {
    for (Observer* observer : m_observers) {
      observer->update(message);
    }
  }

 private:
  std::list<Observer*> m_observers;
};

// ConcreteSubject class
class ConcreteSubject : public Subject {
 public:
  void setState(const std::string_view& newState) {
    state = newState;
    notifyObservers("State has been changed to: " + state);
  }

  [[nodiscard]] std::string getState() const { return state; }

 private:
  std::string state;
};

int main() {
  ConcreteObserver observer1("A");
  ConcreteObserver observer2("B");
  ConcreteObserver observer3("C");

  ConcreteSubject subject;

  subject.addObserver(&observer1);
  subject.addObserver(&observer2);
  subject.addObserver(&observer3);

  subject.setState("Initialized");
  subject.setState("Running");
  subject.setState("Stopped");

  return EXIT_SUCCESS;
}