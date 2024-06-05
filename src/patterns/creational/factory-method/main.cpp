#include <iostream>
#include <memory>

enum class ShapeType : uint8_t { Circle, Square };

// Product
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

// ConcreteProduct
class Circle : public Shape {
 public:
  void draw() override { std::cout << "I am circle\n"; }
  friend class Shape;
};

// ConcreteProduct
class Square : public Shape {
 public:
  void draw() override { std::cout << "I am square\n"; }
  friend class Shape;
};

// Creator
class ShapeCreator {
 public:
  ShapeCreator() = default;
  ShapeCreator(const ShapeCreator&) = default;
  ShapeCreator(ShapeCreator&&) = delete;
  ShapeCreator& operator=(const ShapeCreator&) = default;
  ShapeCreator& operator=(ShapeCreator&&) = delete;
  virtual ~ShapeCreator() = default;
  virtual std::unique_ptr<Shape> create() = 0;
};

// ConcreteCreator
class CircleCreator : public ShapeCreator {
 public:
  std::unique_ptr<Shape> create() override {
    return std::make_unique<Circle>();
  }
};

// ConcreteCreator
class SquareCreator : public ShapeCreator {
 public:
  std::unique_ptr<Shape> create() override {
    return std::make_unique<Square>();
  }
};

int main() {
  std::unique_ptr<ShapeCreator> circle_creator =
      std::make_unique<CircleCreator>();
  std::unique_ptr<Shape> circle = circle_creator->create();
  circle->draw();

  std::unique_ptr<ShapeCreator> square_creator =
      std::make_unique<SquareCreator>();
  std::unique_ptr<Shape> square = square_creator->create();
  square->draw();
}
