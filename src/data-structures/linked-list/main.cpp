#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

#include "singly.h"
#include "doubly.h"
#include "circular.h"

void singly_linked_list() {
  std::cout << "Singly Linked List" << std::endl;
  SinglyLinkedList<int> list;
  list.push_back(5);
  list.push_back(10);
  list.push_back(15);

  std::cout << "Linked List Elements: ";
  list.print();

  list.insert(1, 20);
  std::cout << "Linked List Elements after insert at first 20: ";
  list.print();

  std::cout << "Element at position 2: " << *list.at(2) << std::endl;

  list.push_front(25);
  std::cout << "Linked List Elements after push front 25: ";
  list.print();

  list.erase(1);
  std::cout << "Linked List Elements after erase at 1st index: ";
  list.print();

  list.pop_front();
  std::cout << "Linked List Elements after pop front: ";
  list.print();

  list.pop_back();
  std::cout << "Linked List Elements after pop back: ";
  list.print();

  std::cout << "Linked List size: " << list.size() << std::endl;
}

void doubly_linked_list() {
  std::cout << "Doubly Linked List" << std::endl;
  DoublyLinkedList<int> list;
  list.push_back(5);
  list.push_back(10);
  list.push_back(15);

  std::cout << "Linked List Elements: ";
  list.print();

  list.insert(1, 20);
  std::cout << "Linked List Elements after insert at first 20: ";
  list.print();

  std::cout << "Element at position 2: " << *list.at(2) << std::endl;

  list.push_front(25);
  std::cout << "Linked List Elements after push front 25: ";
  list.print();

  list.erase(1);
  std::cout << "Linked List Elements after erase at 1st index: ";
  list.print();

  list.pop_front();
  std::cout << "Linked List Elements after pop front: ";
  list.print();

  list.pop_back();
  std::cout << "Linked List Elements after pop back: ";
  list.print();

  std::cout << "Linked List size: " << list.size() << std::endl;
}

void circular_linked_list() {
  std::cout << "Circular Linked List" << std::endl;
  CircularLinkedList<int> list;
  list.push_back(5);
  list.push_back(10);
  list.push_back(15);

  std::cout << "Linked List Elements: ";
  list.print();

  list.insert(1, 20);
  std::cout << "Linked List Elements after insert at first 20: ";
  list.print();

  std::cout << "Element at position 2: " << *list.at(2) << std::endl;

  list.push_front(25);
  std::cout << "Linked List Elements after push front 25: ";
  list.print();

  list.erase(1);
  std::cout << "Linked List Elements after erase at 1st index: ";
  list.print();

  list.pop_front();
  std::cout << "Linked List Elements after pop front: ";
  list.print();

  list.pop_back();
  std::cout << "Linked List Elements after pop back: ";
  list.print();

  std::cout << "Linked List size: " << list.size() << std::endl;
}

int main() {
  singly_linked_list();
  std::cout << std::endl;
  doubly_linked_list();
  std::cout << std::endl;
  circular_linked_list();

  return EXIT_SUCCESS;
}
