#include <array>
#include <iostream>
#include <memory>
#define LINUX

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

class LinuxButton : public Widget {
 public:
  void draw() override { std::cout << "LinuxButton\n"; }
};
class LinuxMenu : public Widget {
 public:
  void draw() override { std::cout << "LinuxMenu\n"; }
};

class WindowsButton : public Widget {
 public:
  void draw() override { std::cout << "WindowsButton\n"; }
};
class WindowsMenu : public Widget {
 public:
  void draw() override { std::cout << "WindowsMenu\n"; }
};

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

class LinuxFactory : public Factory {
 public:
  std::unique_ptr<Widget> create_button() override {
    return std::make_unique<LinuxButton>();
  }
  std::unique_ptr<Widget> create_menu() override {
    return std::make_unique<LinuxMenu>();
  }
};

class WindowsFactory : public Factory {
 public:
  std::unique_ptr<Widget> create_button() override {
    return std::make_unique<WindowsButton>();
  }
  std::unique_ptr<Widget> create_menu() override {
    return std::make_unique<WindowsMenu>();
  }
};

class Client {
 private:
  std::shared_ptr<Factory> factory;

 public:
  explicit Client(const std::shared_ptr<Factory> &f) : factory(f) {}

  void draw() {
    std::unique_ptr<Widget> w = factory->create_button();
    w->draw();
    display_window_one();
    display_window_two();
  }

  void display_window_one() {
    std::unique_ptr<Widget> button = factory->create_button();
    std::unique_ptr<Widget> menu = factory->create_menu();
    button->draw();
    menu->draw();
  }

  void display_window_two() {
    std::unique_ptr<Widget> menu = factory->create_menu();
    std::unique_ptr<Widget> button = factory->create_button();
    menu->draw();
    button->draw();
  }
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
