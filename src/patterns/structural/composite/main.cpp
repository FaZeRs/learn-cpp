#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

// Component
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

// Leaf
class Circle : public Graphic {
 public:
  void draw() const override { std::cout << "Drawing Circle\n"; }
};

// Leaf
class Square : public Graphic {
 public:
  void draw() const override { std::cout << "Drawing Square\n"; }
};

// Composite
class GraphicGroup : public Graphic {
 public:
  void add(Graphic* graphic) { m_graphics.emplace_back(graphic); }

  void remove(Graphic* graphic) {
    std::erase(m_graphics, graphic);
  }

  void draw() const override {
    for (const auto* graphic : m_graphics) {
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

  std::cout << "Drawing the main group:\n";
  main_group.draw();

  return EXIT_SUCCESS;
}
