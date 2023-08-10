#include <iostream>
#include <memory>
#include <vector>

// 1. Aggregate
class BookCollection {
 public:
  void addBook(const std::string& book) { m_books.emplace_back(book); }

  [[nodiscard]] std::string getBook(size_t index) const {
    if (index < m_books.size()) {
      return m_books[index];
    }
    return "";
  }

  [[nodiscard]] size_t size() const { return m_books.size(); }

 private:
  std::vector<std::string> m_books;
};

// 2. Iterator
class BookIterator {
 public:
  explicit BookIterator(const std::shared_ptr<BookCollection>& collection)
      : m_collection(collection) {}

  [[nodiscard]] bool hasNext() const {
    return m_current_index < m_collection->size();
  }

  std::string next() {
    if (hasNext()) {
      return m_collection->getBook(m_current_index++);
    }
    return "";
  }

 private:
  std::shared_ptr<BookCollection> m_collection;
  size_t m_current_index{0};
};

int main() {
  // Create a collection and populate it with books
  auto collection = std::make_shared<BookCollection>();
  collection->addBook("Design Patterns");
  collection->addBook("Effective C++");
  collection->addBook("The C++ Standard Library");

  // Iterate over the collection using the iterator
  BookIterator iterator(collection);
  while (iterator.hasNext()) {
    std::cout << iterator.next() << std::endl;
  }

  return 0;
}