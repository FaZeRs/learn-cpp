#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

// Custom hash function
template <typename K, size_t table_size>
struct HashFunction {
  uint64_t operator()(const K& key) const {
    return static_cast<uint64_t>(key) % table_size;
  }
};

// Node to store Key-Value pair
template <typename K, typename V>
struct Node {
  K key;
  V value;
  std::shared_ptr<Node<K, V>> next{nullptr};
  Node(K k, V v) : key(k), value(std::move(v)) {}
};

// Hash Map Class
template <typename K, typename V, size_t table_size,
          typename F = HashFunction<K, table_size>>
class HashMap {
 public:
  HashMap()
      // NOLINTNEXTLINE
      : m_table(std::make_unique<std::shared_ptr<Node<K, V>>[]>(table_size)) {}

  constexpr V& operator[](const K& key) {
    uint64_t index = hashFunction(key);
    auto curr = m_table[index];
    while (curr) {
      if (curr->key == key) {
        return curr->value;
      }
      curr = curr->next;
    }
    throw std::runtime_error("Key not found");
  }

  void put(const K& key, const V& value) {
    uint64_t index = hashFunction(key);
    auto& node = m_table[index];
    if (node == nullptr) {
      node = std::make_shared<Node<K, V>>(key, value);
    } else {
      auto curr = node;
      while (curr) {
        if (curr->key == key) {
          curr->value = value;
          return;
        }
        if (curr->next == nullptr) break;
        curr = curr->next;
      }
      curr->next = std::make_shared<Node<K, V>>(key, value);
    }
  }

  void remove(const K& key) {
    uint64_t index = hashFunction(key);
    auto& node = m_table[index];
    if (node == nullptr) return;

    if (node->key == key) {
      node = node->next;
      return;
    }

    auto prev = node;
    auto curr = node->next;
    while (curr) {
      if (curr->key == key) {
        prev->next = curr->next;
        return;
      }
      prev = curr;
      curr = curr->next;
    }
  }

 private:
  F hashFunction;
  // NOLINTNEXTLINE
  std::unique_ptr<std::shared_ptr<Node<K, V>>[]> m_table;
};

int main() {
  HashMap<int, std::string, 10> hmap;
  hmap.put(1, "1");
  hmap.put(2, "2");
  hmap.put(3, "3");

  assert(hmap[1] == "1");
  assert(hmap[2] == "2");
  assert(hmap[3] == "3");

  hmap.remove(3);
  try {
    hmap[3];
    assert(false);
  } catch (const std::runtime_error& /*e*/) {
    assert(true);
  }

  return EXIT_SUCCESS;
}
