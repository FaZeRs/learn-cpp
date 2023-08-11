#include <iostream>
#include <memory>

class Image {
 public:
  Image() = default;
  virtual ~Image() = default;
  Image(const Image &) = default;
  Image &operator=(const Image &) = default;
  Image(Image &&) = default;
  Image &operator=(Image &&) = default;

  virtual void display() = 0;
};

class RealImage : public Image {
 public:
  explicit RealImage(std::string filename) : m_filename(std::move(filename)) {
    loadFromDisk();
  }

  void display() override {
    std::cout << "Displaying " << m_filename << std::endl;
  }

 private:
  void loadFromDisk() const {
    std::cout << "Loading " << m_filename << std::endl;
  }

 private:
  std::string m_filename;
};

class ProxyImage : public Image {
 public:
  explicit ProxyImage(std::string filename) : m_filename(std::move(filename)) {}

  void display() override {
    if (!m_real_image) {
      m_real_image = std::make_unique<RealImage>(m_filename);
    }
    m_real_image->display();
  }

 private:
  std::unique_ptr<RealImage> m_real_image;
  std::string m_filename;
};

int main() {
  std::unique_ptr<Image> image1 =
      std::make_unique<ProxyImage>("testImage1.jpg");
  std::unique_ptr<Image> image2 =
      std::make_unique<ProxyImage>("testImage2.jpg");

  std::cout << "First call to display() method:" << std::endl;
  // Image will be loaded and displayed only when the display method is called.
  image1->display();
  image2->display();

  std::cout << "Second call to display() method:" << std::endl;
  // The image is already loaded, so it will only be displayed this time.
  image1->display();

  return 0;
}