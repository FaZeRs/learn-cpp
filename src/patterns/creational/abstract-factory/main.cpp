#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

#define LINUX

// Abstract Product
class Widget {
 public:
  Widget() = default;
  virtual ~Widget() = default;
  Widget(const Widget &) = default;
  Widget &operator=(const Widget &) = default;
  Widget(Widget &&) = default;
  Widget &operator=(Widget &&) = default;

  virtual void draw() = 0;
};

// Concrete Product
class LinuxButton : public Widget {
 public:
  void draw() override { std::cout << "LinuxButton" << std::endl; }
};

// Concrete Product
class LinuxMenu : public Widget {
 public:
  void draw() override { std::cout << "LinuxMenu" << std::endl; }
};

// Concrete Product
class WindowsButton : public Widget {
 public:
  void draw() override { std::cout << "WindowsButton" << std::endl; }
};

// Concrete Product
class WindowsMenu : public Widget {
 public:
  void draw() override { std::cout << "WindowsMenu" << std::endl; }
};

// Abstract Factory
class Factory {
 public:
  Factory() = default;
  virtual ~Factory() = default;
  Factory(const Factory &) = default;
  Factory &operator=(const Factory &) = default;
  Factory(Factory &&) = default;
  Factory &operator=(Factory &&) = default;
  virtual std::unique_ptr<Widget> create_button() = 0;
  virtual std::unique_ptr<Widget> create_menu() = 0;
};

// Concrete Factory
class LinuxFactory : public Factory {
 public:
  std::unique_ptr<Widget> create_button() override {
    return std::make_unique<LinuxButton>();
  }
  std::unique_ptr<Widget> create_menu() override {
    return std::make_unique<LinuxMenu>();
  }
};

// Concrete Factory
class WindowsFactory : public Factory {
 public:
  std::unique_ptr<Widget> create_button() override {
    return std::make_unique<WindowsButton>();
  }
  std::unique_ptr<Widget> create_menu() override {
    return std::make_unique<WindowsMenu>();
  }
};

// Client
class Client {
 public:
  explicit Client(std::shared_ptr<Factory> f) : m_factory(std::move(f)) {}

  void draw() const {
    std::unique_ptr<Widget> w = m_factory->create_button();
    w->draw();
    display_window_one();
    display_window_two();
  }

  void display_window_one() const {
    std::unique_ptr<Widget> button = m_factory->create_button();
    std::unique_ptr<Widget> menu = m_factory->create_menu();
    button->draw();
    menu->draw();
  }

  void display_window_two() const {
    std::unique_ptr<Widget> menu = m_factory->create_menu();
    std::unique_ptr<Widget> button = m_factory->create_button();
    menu->draw();
    button->draw();
  }

 private:
  std::shared_ptr<Factory> m_factory;
};

int main() {
  std::shared_ptr<Factory> factory = nullptr;
#ifdef LINUX
  factory = std::make_shared<LinuxFactory>();
#else  // WINDOWS
  factory = std::make_shared<WindowsFactory>();
#endif

  auto c = std::make_unique<Client>(factory);
  c->draw();

  return EXIT_SUCCESS;
}
