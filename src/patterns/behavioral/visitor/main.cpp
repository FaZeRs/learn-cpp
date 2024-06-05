#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdlib>
#include <iostream>

// Forward declaration for the Visitor class
class ShapeVisitor;

// Element
class Shape {
 public:
  Shape() = default;
  virtual ~Shape() = default;
  Shape(const Shape&) = default;
  Shape& operator=(const Shape&) = default;
  Shape(Shape&&) = default;
  Shape& operator=(Shape&&) = default;

  virtual void accept(ShapeVisitor& visitor) = 0;
};

// ConcreteElement
class Circle : public Shape {
 public:
  explicit Circle(double radius) : m_radius(radius) {}
  [[nodiscard]] double getRadius() const { return m_radius; }
  void accept(ShapeVisitor& visitor) override;

 private:
  double m_radius;
};

// ConcreteElement
class Rectangle : public Shape {
 public:
  Rectangle(double width, double height) : m_width(width), m_height(height) {}
  [[nodiscard]] double getWidth() const { return m_width; }
  [[nodiscard]] double getHeight() const { return m_height; }
  void accept(ShapeVisitor& visitor) override;

 private:
  double m_width;
  double m_height;
};

// Visitor
class ShapeVisitor {
 public:
  ShapeVisitor() = default;
  virtual ~ShapeVisitor() = default;
  ShapeVisitor(const ShapeVisitor&) = default;
  ShapeVisitor& operator=(const ShapeVisitor&) = default;
  ShapeVisitor(ShapeVisitor&&) = default;
  ShapeVisitor& operator=(ShapeVisitor&&) = default;

  virtual void visit(Circle& circle) = 0;
  virtual void visit(Rectangle& rectangle) = 0;
};

// ConcreteVisitor
class AreaVisitor : public ShapeVisitor {
 public:
  [[nodiscard]] double getResult() const { return m_result; }

  void visit(Circle& circle) override {
    m_result = M_PI * std::pow(circle.getRadius(), 2);
  }

  void visit(Rectangle& rectangle) override {
    m_result = rectangle.getWidth() * rectangle.getHeight();
  }

 private:
  double m_result = 0.0;
};

// ConcreteVisitor
class PerimeterVisitor : public ShapeVisitor {
 public:
  [[nodiscard]] double getResult() const { return m_result; }

  void visit(Circle& circle) override {
    m_result = 2 * M_PI * circle.getRadius();
  }

  void visit(Rectangle& rectangle) override {
    m_result = 2 * (rectangle.getWidth() + rectangle.getHeight());
  }

 private:
  double m_result = 0.0;
};

void Circle::accept(ShapeVisitor& visitor) { visitor.visit(*this); }

void Rectangle::accept(ShapeVisitor& visitor) { visitor.visit(*this); }

int main() {
  Circle circle(5);
  Rectangle rectangle(4, 6);

  AreaVisitor area_visitor;
  PerimeterVisitor perimeter_visitor;

  circle.accept(area_visitor);
  std::cout << "Area of circle: " << area_visitor.getResult() << "\n";

  rectangle.accept(area_visitor);
  std::cout << "Area of rectangle: " << area_visitor.getResult() << "\n";

  circle.accept(perimeter_visitor);
  std::cout << "Perimeter of circle: " << perimeter_visitor.getResult() << "\n";

  rectangle.accept(perimeter_visitor);
  std::cout << "Perimeter of rectangle: " << perimeter_visitor.getResult()
            << "\n";

  return EXIT_SUCCESS;
}
