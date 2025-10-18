#include <print>
#include <type_traits>
#include <utility>

// A helper to check if a type has a member function `foo`.
template <typename T>
class has_foo {
 private:
  template <typename U>
  static auto check(int) -> decltype(std::declval<U>().foo(), std::true_type{});

  template <typename>
  static std::false_type check(...);

 public:
  // cppcheck-suppress unusedStructMember
  static constexpr bool value = decltype(check<T>(0))::value;
};

// The primary template for `callFoo` will be enabled if T has `foo`.
template <typename T>
  requires has_foo<T>::value
void callFoo(T& t) {
  t.foo();
}

// The specialization of `callFoo` will be enabled if T does not have `foo`.
template <typename T>
  requires(!has_foo<T>::value)
void callFoo(T&) {
  std::println("foo() not available");
}

class A {
 public:
  // cppcheck-suppress functionStatic
  void foo() const { std::println("A::foo()"); }
};

class B {
 public:
  // cppcheck-suppress functionStatic
  void bar() const { std::println("B::bar()"); }
};

int main() {
  A a;
  B b;

  callFoo(a);  // This will call A::foo()
  callFoo(b);  // This will print "foo() not available"

  return 0;
}