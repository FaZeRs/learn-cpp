#include <algorithm>
#include <functional>
#include <memory>
#include <print>
#include <ranges>
#include <source_location>
#include <type_traits>
#include <vector>

template <typename T>
concept Emittable = std::movable<T>;

template <typename... Args>
  requires(Emittable<Args> && ...)
class Signal;

class Connection {
 public:
  constexpr Connection() = default;
  constexpr Connection(const Connection&) noexcept = default;
  constexpr Connection& operator=(const Connection&) noexcept = default;
  constexpr Connection(Connection&&) noexcept = default;
  constexpr Connection& operator=(Connection&&) noexcept = default;
  constexpr ~Connection() = default;
  constexpr void disconnect() noexcept { connected_ = false; }
  [[nodiscard]] constexpr bool isConnected() const noexcept {
    return connected_;
  }

 private:
  bool connected_{true};
};

template <typename T, typename... Args>
concept Invocable = std::is_invocable_v<T, Args...>;

template <typename T, typename F, typename... Args>
concept MemberFunction = std::is_member_function_pointer_v<F> &&
                         std::is_invocable_r_v<void, F, T*, Args...>;

template <typename... Args>
  requires(Emittable<Args> && ...)
class Signal {
 public:
  using SlotType = std::move_only_function<void(Args...) const>;

  constexpr Signal() noexcept = default;
  Signal(const Signal&) = delete;
  Signal& operator=(const Signal&) = delete;
  constexpr Signal(Signal&&) noexcept = default;
  constexpr Signal& operator=(Signal&&) noexcept = default;
  constexpr ~Signal() = default;

  template <typename T, typename F>
    requires MemberFunction<T, F, Args...>
  constexpr Connection connect(T* instance, F memberFunction,
                               const std::source_location& location =
                                   std::source_location::current()) noexcept {
    auto slot = [instance, memberFunction](Args... args) {
      std::invoke(memberFunction, instance, args...);
    };
    return connectImpl(std::move(slot), location);
  }

  template <typename F>
    requires Invocable<F, Args...>
  constexpr Connection connect(F&& slot,
                               const std::source_location& location =
                                   std::source_location::current()) noexcept {
    return connectImpl(std::forward<F>(slot), location);
  }

  constexpr void emit(const Args&... args) const noexcept {
    std::span slots{slots_};
    auto connected_slots =
        slots | std::views::filter([](const auto& slot) {
          return slot.connection && slot.connection->isConnected();
        });

    for (const auto& slot : connected_slots) {
      slot.callback(args...);
    }
  }

  constexpr void operator()(const Args&... args) const noexcept {
    emit(args...);
  }

 private:
  struct SlotEntry {
    [[no_unique_address]] SlotType callback;
    std::shared_ptr<Connection> connection;
    [[no_unique_address]] std::source_location location;
  };

  Connection connectImpl(SlotType slot,
                         const std::source_location& location) noexcept {
    auto conn = std::make_shared<Connection>();
    slots_.push_back(SlotEntry{std::move(slot), conn, location});
    return *conn;
  }

  mutable std::vector<SlotEntry> slots_;
};

template <typename... Args>
Signal(Signal<Args...>) -> Signal<Args...>;

class Button {
 public:
  Signal<> clicked;
  Signal<int, int> positionChanged;

  constexpr void click() const { clicked.emit(); }

  constexpr void setPosition(int x, int y) { positionChanged.emit(x, y); }
};

class Window {
 public:
  // cppcheck-suppress functionStatic
  constexpr void handleClick() const { std::println("Button clicked!"); }

  // cppcheck-suppress functionStatic
  constexpr void handlePositionChange(int x, int y) const {
    std::println("Button position changed to: ({}, {})", x, y);
  }
};

int main() {
  Button button;
  Window window;

  button.clicked.connect(&window, &Window::handleClick);
  button.positionChanged.connect(&window, &Window::handlePositionChange);

  auto conn3 = button.clicked.connect(
      []() { std::println("Lambda handler: Button clicked!"); });

  button.click();
  button.setPosition(100, 200);

  conn3.disconnect();

  button.click();

  return 0;
}
