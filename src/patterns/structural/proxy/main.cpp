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
  std::string filename;

 public:
  explicit RealImage(std::string filename) : filename(std::move(filename)) {
    loadFromDisk();
  }

  void display() override {
    std::cout << "Displaying " << filename << std::endl;
  }

 private:
  void loadFromDisk() const {
    std::cout << "Loading " << filename << std::endl;
  }
};

class ProxyImage : public Image {
  std::unique_ptr<RealImage> realImage;
  std::string filename;

 public:
  explicit ProxyImage(std::string filename) : filename(std::move(filename)) {}

  void display() override {
    if (!realImage) {
      realImage = std::make_unique<RealImage>(filename);
    }
    realImage->display();
  }
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