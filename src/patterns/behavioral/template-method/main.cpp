#include <iostream>

// Abstract class with the template method
class DocumentReader {
 public:
  DocumentReader() = default;
  virtual ~DocumentReader() = default;
  DocumentReader(const DocumentReader&) = default;
  DocumentReader& operator=(const DocumentReader&) = default;
  DocumentReader(DocumentReader&&) = default;
  DocumentReader& operator=(DocumentReader&&) = default;

  void readDocument() {
    openDocument();
    processContent();
    closeDocument();
  }

  // Default implementations for these methods. Derived classes can override
  // these.
  virtual void openDocument() {
    std::cout << "Default method to open a document.\n";
  }
  virtual void processContent() = 0;  // Pure virtual method
  virtual void closeDocument() {
    std::cout << "Default method to close a document.\n";
  }
};

// ConcreteClass
class PDFReader : public DocumentReader {
 public:
  void openDocument() override { std::cout << "Opening a PDF document.\n"; }

  void processContent() override {
    std::cout << "Processing content of a PDF document.\n";
  }

  void closeDocument() override { std::cout << "Closing a PDF document.\n"; }
};

// ConcreteClass
class WordReader : public DocumentReader {
 public:
  void openDocument() override { std::cout << "Opening a Word document.\n"; }

  void processContent() override {
    std::cout << "Processing content of a Word document.\n";
  }

  // We don't need to override closeDocument() here. It will use the default
  // implementation from the base class.
};

int main() {
  PDFReader pdf_reader;
  WordReader word_reader;

  std::cout << "Using PDF Reader:\n";
  pdf_reader.readDocument();

  std::cout << "\nUsing Word Reader:\n";
  word_reader.readDocument();

  return 0;
}
