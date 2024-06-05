#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

// Flyweight
class CharacterStyle {
 public:
  CharacterStyle(std::string font, int size, std::string color)
      : font_(std::move(font)), size_(size), color_(std::move(color)) {}

  void print() const {
    std::cout << "Font: " << font_ << ", Size: " << size_
              << ", Color: " << color_ << "\n";
  }

 private:
  std::string font_;
  int size_;
  std::string color_;
};

// FlyweightFactory
class CharacterStyleFactory {
 public:
  std::shared_ptr<CharacterStyle> getStyle(const std::string& font, int size,
                                           const std::string& color) {
    std::string key = font + std::to_string(size) + color;
    auto it = style_pool_.find(key);
    if (it != style_pool_.end()) {
      return it->second;
    } else {
      auto style = std::make_shared<CharacterStyle>(font, size, color);
      style_pool_[key] = style;
      return style;
    }
  }

 private:
  std::unordered_map<std::string, std::shared_ptr<CharacterStyle>> style_pool_;
};

class Character {
 public:
  Character(char symbol, std::shared_ptr<CharacterStyle> style)
      : symbol_(symbol), style_(std::move(style)) {}

  void print() const {
    std::cout << "Character: " << symbol_ << ", Style: ";
    style_->print();
  }

 private:
  char symbol_;
  std::shared_ptr<CharacterStyle> style_;
};

int main() {
  CharacterStyleFactory style_factory;

  // Obtain shared styles
  auto style1 = style_factory.getStyle("Arial", 12, "Red");
  auto style2 = style_factory.getStyle("Times New Roman", 14, "Blue");

  // Create characters with shared styles
  Character charA('A', style1);
  Character charB('B', style1);  // Reuses the style of charA
  Character charC('C', style2);

  // Display characters and styles
  charA.print();
  charB.print();
  charC.print();

  return EXIT_SUCCESS;
}
