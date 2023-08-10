#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class HtmlBuilder;

class HtmlElement {
  std::string m_name;
  std::string m_text;
  std::vector<HtmlElement> m_elements;
  constexpr static size_t m_indent_size = 4;

  HtmlElement() = default;
  HtmlElement(std::string name, std::string text)
      : m_name(std::move(name)), m_text(std::move(text)) {}
  friend class HtmlBuilder;

 public:
  std::string str(size_t indent = 0) {
    std::ostringstream oss;
    oss << std::string(m_indent_size * indent, ' ') << "<" << m_name << ">"
        << std::endl;

    if (m_text.size()) {
      oss << std::string(m_indent_size * (indent + 1), ' ') << m_text
          << std::endl;
    }

    for (auto &element : m_elements) oss << element.str(indent + 1);

    oss << std::string(m_indent_size * indent, ' ') << "</" << m_name << ">"
        << std::endl;
    return oss.str();
  }
};

class HtmlBuilder {
  HtmlElement m_root;

 public:
  explicit HtmlBuilder(const std::string_view &root_name) {
    m_root.m_name = root_name;
  }
  HtmlBuilder &add_child(const std::string &child_name,
                         const std::string &content) {
    m_root.m_elements.emplace_back(HtmlElement{child_name, content});
    return *this;
  }
  std::string str() { return m_root.str(); }
  explicit operator HtmlElement() const { return m_root; }
};

int main() {
  auto builder = HtmlBuilder{"ul"};
  builder.add_child("li", "hello").add_child("li", "world");
  std::cout << builder.str() << std::endl;

  return EXIT_SUCCESS;
}