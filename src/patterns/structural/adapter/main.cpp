#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#define LEGACY

// Adaptee
class OldPrinter {
 public:
  void printDocument(const std::string &document) const {
    std::cout << "Old Printer: " << m_model << ", document: " << document
              << "\n";
  }

 private:
  std::string m_model{"HP LaserJet 1018"};
};

// Target
class DocumentPrinter {
 public:
  DocumentPrinter() = default;
  virtual ~DocumentPrinter() = default;
  DocumentPrinter(const DocumentPrinter &) = default;
  DocumentPrinter &operator=(const DocumentPrinter &) = default;
  DocumentPrinter(DocumentPrinter &&) = default;
  DocumentPrinter &operator=(DocumentPrinter &&) = default;

  virtual void print(const std::string &document) const = 0;
};

class ModernPrinter : public DocumentPrinter {
 public:
  void print(const std::string &document) const override {
    std::cout << "Modern Printer: " << m_model << ", document: " << document
              << "\n";
  }

 private:
  std::string m_model{"HP LaserJet 2000"};
};

// Adapter
class AdapterOldPrinter : public DocumentPrinter {
 public:
  void print(const std::string &document) const override {
    m_old_printer.printDocument(document);
  }

 private:
  OldPrinter m_old_printer;
};

int main() {
  std::unique_ptr<DocumentPrinter> printer = nullptr;
#ifdef LEGACY
  printer = std::make_unique<AdapterOldPrinter>();
#else
  printer = std::make_unique<ModernPrinter>();
#endif

  printer->print("Document for printer.");

  return EXIT_SUCCESS;
}
