#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

class Graphic {
 public:
  Graphic() = default;
  virtual ~Graphic() = default;
  Graphic(const Graphic&) = default;
  Graphic& operator=(const Graphic&) = default;
  Graphic(Graphic&&) = default;
  Graphic& operator=(Graphic&&) = default;

  virtual void draw() const = 0;
};

class Circle : public Graphic {
 public:
  void draw() const override { std::cout << "Drawing Circle" << std::endl; }
};

class Square : public Graphic {
 public:
  void draw() const override { std::cout << "Drawing Square" << std::endl; }
};

class GraphicGroup : public Graphic {
 public:
  void add(Graphic* graphic) { m_graphics.emplace_back(graphic); }

  void remove(Graphic* graphic) {
    m_graphics.erase(std::remove(m_graphics.begin(), m_graphics.end(), graphic),
                     m_graphics.end());
  }

  void draw() const override {
    for (const auto& graphic : m_graphics) {
      graphic->draw();
    }
  }

 private:
  std::vector<Graphic*> m_graphics;
};

int main() {
  Circle circle;
  Square square;

  GraphicGroup group;
  group.add(&circle);
  group.add(&square);

  GraphicGroup main_group;
  main_group.add(&group);

  std::cout << "Drawing the main group:" << std::endl;
  main_group.draw();

  return EXIT_SUCCESS;
}