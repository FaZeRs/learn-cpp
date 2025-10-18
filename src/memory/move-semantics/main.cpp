#include <concepts>
#include <memory>
#include <print>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// Class demonstrating move semantics
class Resource {
 public:
  // Constructor
  explicit constexpr Resource(std::string_view data = "") : data_(data) {
    std::println("Constructor called for: {}", data_);
  }

  // Copy constructor
  constexpr Resource(const Resource& other) : data_(other.data_) {
    std::println("Copy constructor called for: {}", data_);
  }

  // Move constructor
  constexpr Resource(Resource&& other) noexcept
      : data_(std::exchange(other.data_, "")) {
    std::println("Move constructor called for: {}", data_);
  }

  // Copy assignment
  constexpr Resource& operator=(const Resource& other) {
    std::println("Copy assignment called for: {}", other.data_);
    if (this != &other) {
      data_ = other.data_;
    }
    return *this;
  }

  // Move assignment
  constexpr Resource& operator=(Resource&& other) noexcept {
    std::println("Move assignment called for: {}", other.data_);
    if (this != &other) {
      data_ = std::exchange(other.data_, "");
    }
    return *this;
  }

  // Destructor
  ~Resource() { std::println("Destructor called for: {}", data_); }

  [[nodiscard]] const std::string& data() const { return data_; }

 private:
  std::string data_;
};

// Perfect forwarding wrapper class
template <typename T>
  requires std::movable<T>
class Container {
 public:
  // Perfect forwarding constructor
  template <typename... Args>
  explicit Container(Args&&... args)
      : resource_(std::forward<Args>(args)...) {  // NOLINT
    std::println("Container constructed with perfect forwarding");
  }
  ~Container() = default;

  // Move constructor
  Container(Container&& other) noexcept
      : resource_(std::move(other.resource_)) {
    std::println("Container move constructor called");
  }

  // Prevent copying
  Container(const Container&) = delete;
  Container& operator=(const Container&) = delete;

  // Allow moving
  Container& operator=(Container&& other) noexcept {
    std::println("Container move assignment called");
    if (this != &other) {
      resource_ = std::move(other.resource_);
    }
    return *this;
  }

  [[nodiscard]] const T& get() const { return resource_; }

 private:
  T resource_;
};

// Function demonstrating perfect forwarding
template <typename T, typename... Args>
  requires std::constructible_from<T, Args...>
[[nodiscard]] constexpr Container<T> make_container(Args&&... args) {
  return Container<T>(std::forward<Args>(args)...);
}

// Function that takes an rvalue reference
void process_resource(Resource&& res) {
  std::println("Processing rvalue resource: {}", res.data());
  // Resource is moved into the vector
  std::vector<Resource> resources;
  resources.push_back(std::move(res));
}

int main() {
  std::println("=== Basic Move Semantics ===");
  Resource r1("Original");
  Resource r2 = std::move(r1);  // Move constructor
  // cppcheck-suppress accessMoved
  std::println("r1 data after move: {}", r1.data());  // NOLINT
  std::println("r2 data after move: {}", r2.data());

  std::println("\n=== Perfect Forwarding ===");
  // Forwards arguments to Resource constructor
  auto container1 = make_container<Resource>("Direct construction");

  // Moving an existing resource into the container
  Resource temp("Temporary");
  auto container2 = make_container<Resource>(std::move(temp));

  std::println("\n=== Vector of Resources ===");
  std::vector<Resource> resources;
  resources.reserve(2);  // Prevent reallocation

  // Emplace back constructs in place
  resources.emplace_back("First");

  // Push back moves the resource
  Resource r3("Second");
  resources.push_back(std::move(r3));

  std::println("\n=== Processing Rvalue ===");
  process_resource(Resource("Temporary Resource"));

  return 0;
}