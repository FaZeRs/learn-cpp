#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

// Implementor
class Renderer {
 public:
  Renderer() = default;
  virtual ~Renderer() = default;
  Renderer(const Renderer&) = default;
  Renderer& operator=(const Renderer&) = default;
  Renderer(Renderer&&) = default;
  Renderer& operator=(Renderer&&) = default;

  virtual void renderCircle(float x, float y, float radius) = 0;
};

// ConcreteImplementor
class RasterRenderer : public Renderer {
 public:
  void renderCircle(float x, float y, float radius) override {
    std::cout << "Rasterizing circle at position: " << x << ", " << y
              << " of radius: " << radius << std::endl;
  }
};

// ConcreteImplementor
class VectorRenderer : public Renderer {
 public:
  void renderCircle(float x, float y, float radius) override {
    std::cout << "Drawing vector circle at position: " << x << ", " << y
              << " of radius: " << radius << std::endl;
  }
};

// Abstraction
class Shape {
 public:
  explicit Shape(std::shared_ptr<Renderer> renderer)
      : m_renderer(std::move(renderer)) {}
  virtual ~Shape() = default;
  Shape(const Shape&) = default;
  Shape& operator=(const Shape&) = default;
  Shape(Shape&&) = default;
  Shape& operator=(Shape&&) = default;

  virtual void draw() = 0;
  [[nodiscard]] Renderer* getRenderer() const { return m_renderer.get(); }

 private:
  std::shared_ptr<Renderer> m_renderer;
};

// RefinedAbstraction
class Circle : public Shape {
 public:
  Circle(std::shared_ptr<Renderer> renderer, float x, float y, float radius)
      : Shape(std::move(renderer)), m_x(x), m_y(y), m_radius(radius) {}

  void draw() override { getRenderer()->renderCircle(m_x, m_y, m_radius); }
  void resize(float factor) { m_radius *= factor; }

 private:
  float m_x;
  float m_y;
  float m_radius;
};

int main() {
  auto raster_renderer = std::make_shared<RasterRenderer>();
  Circle raster_rircle(raster_renderer, 5, 5, 10);
  raster_rircle.draw();
  raster_rircle.resize(2);
  raster_rircle.draw();

  auto vector_renderer = std::make_shared<VectorRenderer>();
  Circle vector_circle(vector_renderer, 5, 5, 10);
  vector_circle.draw();
  vector_circle.resize(2);
  vector_circle.draw();

  return EXIT_SUCCESS;
}
