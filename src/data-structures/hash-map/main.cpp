#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <memory>
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

  bool get(const K& key, V& value) {
    uint64_t index = hashFunction(key);
    auto curr = m_table[index];
    while (curr) {
      if (curr->key == key) {
        value = curr->value;
        return true;
      }
      curr = curr->next;
    }
    return false;
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

  std::string value;
  bool result = hmap.get(2, value);
  assert(result);
  assert(value == "2");

  result = hmap.get(3, value);
  assert(result);
  assert(value == "3");

  hmap.remove(3);
  result = hmap.get(3, value);
  assert(!result);

  return EXIT_SUCCESS;
}
