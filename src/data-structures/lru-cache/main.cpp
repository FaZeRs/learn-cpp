#include <concepts>
#include <iostream>
#include <list>
#include <optional>
#include <print>
#include <string_view>
#include <unordered_map>

template <typename T>
concept HashableKey = std::regular<T> && requires(T a) {
  { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

template <HashableKey Key, typename Value>
class LRUCache {
 public:
  using key_type = Key;
  using value_type = Value;
  using list_type = std::list<std::pair<key_type, value_type>>;
  using map_type = std::unordered_map<key_type, typename list_type::iterator>;

  constexpr explicit LRUCache(size_t size) noexcept : capacity(size) {}

  [[nodiscard]] constexpr std::optional<value_type> get(const key_type& key) {
    auto it = cache_map.find(key);
    if (it == cache_map.end()) {
      return std::nullopt;
    }

    cache_list.splice(cache_list.begin(), cache_list, it->second);
    return it->second->second;
  }

  constexpr void put(const key_type& key, const value_type& value) {
    if (auto it = cache_map.find(key); it != cache_map.end()) {
      it->second->second = std::move(value);
      cache_list.splice(cache_list.begin(), cache_list, it->second);
      return;
    }

    if (full()) {
      evict_oldest();
    }

    cache_list.emplace_front(key, std::move(value));
    cache_map[key] = cache_list.begin();
  }

  template <typename... Args>
  constexpr void emplace(const key_type& key, Args&&... args) {
    if (auto it = cache_map.find(key); it != cache_map.end()) {
      it->second->second = value_type(std::forward<Args>(args)...);
      cache_list.splice(cache_list.begin(), cache_list, it->second);
      return;
    }

    if (full()) {
      evict_oldest();
    }

    cache_list.emplace_front(key, value_type(std::forward<Args>(args)...));
    cache_map[key] = cache_list.begin();
  }

  [[nodiscard]] constexpr size_t size() const noexcept {
    return cache_map.size();
  }
  [[nodiscard]] constexpr auto begin() const noexcept {
    return cache_list.begin();
  }
  [[nodiscard]] constexpr auto end() const noexcept { return cache_list.end(); }
  [[nodiscard]] constexpr auto cbegin() const noexcept {
    return cache_list.cbegin();
  }
  [[nodiscard]] constexpr auto cend() const noexcept {
    return cache_list.cend();
  }

  [[nodiscard]] constexpr bool contains(const key_type& key) const {
    return cache_map.contains(key);
  }

  constexpr void clear() noexcept {
    cache_map.clear();
    cache_list.clear();
  }

  [[nodiscard]] constexpr size_t max_size() const noexcept { return capacity; }
  [[nodiscard]] constexpr bool empty() const noexcept {
    return cache_map.empty();
  }
  [[nodiscard]] constexpr bool full() const noexcept {
    return cache_map.size() == capacity;
  }

 private:
  size_t capacity;
  list_type cache_list;
  map_type cache_map;

  constexpr void evict_oldest() {
    const auto& [last_key, _] = cache_list.back();
    cache_map.erase(last_key);
    cache_list.pop_back();
  }
};

int main() {
  using namespace std::literals;

  LRUCache<std::string_view, int> cache(3);

  cache.put("one"sv, 1);
  cache.put("two"sv, 2);
  cache.put("three"sv, 3);

  std::println("Contains 'one': {}", cache.contains("one"sv));
  std::println("Is full: {}", cache.full());

  if (auto value = cache.get("one"sv)) {
    std::println("one: {}", *value);
  }

  cache.put("four"sv, 4);

  if (auto value = cache.get("two"sv)) {
    std::println("two: {}", *value);
  } else {
    std::println("two was evicted");
  }

  cache.emplace("five"sv, 5);

  std::println("\nAll cache entries:");
  for (const auto& [key, value] : cache) {
    std::println("{}: {}", key, value);
  }

  return 0;
}
