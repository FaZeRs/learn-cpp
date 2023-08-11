#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

// Memento class
class Memento {
 public:
  explicit Memento(std::string s) : m_state(std::move(s)) {}
  [[nodiscard]] std::string getState() const { return m_state; }

 private:
  std::string m_state;
};

// Originator class
class Editor {
 public:
  void type(const std::string& words) { m_text += " " + words; }

  // Saves the current state
  [[nodiscard]] Memento save() const { return Memento(m_text); }

  // Restores the saved state
  void restore(const Memento& m) { m_text = m.getState(); }

  void printContent() const { std::cout << m_text << std::endl; }

 private:
  std::string m_text;
};

// Caretaker class
class Caretaker {
 public:
  void save(const Editor& editor) { m_mementos.emplace_back(editor.save()); }

  void undo(Editor& editor) {
    if (m_mementos.empty()) {
      std::cout << "Caretaker: No saved states to undo!" << std::endl;
      return;
    }
    m_mementos.pop_back();
    editor.restore(m_mementos.back());
  }

 private:
  std::vector<Memento> m_mementos;
};

int main() {
  Editor editor;
  Caretaker caretaker;

  caretaker.save(editor);
  editor.type("Hello,");
  caretaker.save(editor);

  editor.type("world!");
  caretaker.save(editor);

  editor.printContent();  // Outputs: Hello, world!

  caretaker.undo(editor);
  editor.printContent();  // Outputs: Hello,

  caretaker.undo(editor);
  editor.printContent();  // Outputs: (empty string)

  return EXIT_SUCCESS;
}