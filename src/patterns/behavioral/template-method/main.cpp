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
    std::cout << "Default method to open a document." << std::endl;
  }
  virtual void processContent() = 0;  // Pure virtual method
  virtual void closeDocument() {
    std::cout << "Default method to close a document." << std::endl;
  }
};

class PDFReader : public DocumentReader {
 public:
  void openDocument() override {
    std::cout << "Opening a PDF document." << std::endl;
  }

  void processContent() override {
    std::cout << "Processing content of a PDF document." << std::endl;
  }

  void closeDocument() override {
    std::cout << "Closing a PDF document." << std::endl;
  }
};

class WordReader : public DocumentReader {
 public:
  void openDocument() override {
    std::cout << "Opening a Word document." << std::endl;
  }

  void processContent() override {
    std::cout << "Processing content of a Word document." << std::endl;
  }

  // We don't need to override closeDocument() here. It will use the default
  // implementation from the base class.
};

int main() {
  PDFReader pdf_reader;
  WordReader word_reader;

  std::cout << "Using PDF Reader:" << std::endl;
  pdf_reader.readDocument();

  std::cout << std::endl << "Using Word Reader:" << std::endl;
  word_reader.readDocument();

  return 0;
}