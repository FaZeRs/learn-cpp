#include <iostream>
#include <memory>
#include <string>
#include <vector>

enum class ShapeType { Circle, Square };

class Shape {
 public:
  Shape() = default;
  Shape(const Shape&) = default;
  Shape& operator=(const Shape&) = default;
  Shape(Shape&&) = default;
  Shape& operator=(Shape&&) = default;

  virtual ~Shape() = default;
  virtual void draw() = 0;
};

class Circle : public Shape {
 public:
  void draw() override { std::cout << "I am circle" << std::endl; }
  friend class Shape;
};

class Square : public Shape {
 public:
  void draw() override { std::cout << "I am square" << std::endl; }
  friend class Shape;
};

class ShapeFactory {
 public:
  static std::unique_ptr<Shape> create(const ShapeType& type) {
    switch (type) {
      case ShapeType::Circle:
        return std::make_unique<Circle>();
      case ShapeType::Square:
        return std::make_unique<Square>();
    }
    return nullptr;
  }
};

int main() {
  std::unique_ptr<Shape> circle = ShapeFactory::create(ShapeType::Circle);
  std::unique_ptr<Shape> square = ShapeFactory::create(ShapeType::Square);
  circle->draw();
  square->draw();
}