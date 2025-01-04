#include <X11/Xlib.h>

#include <expected>
#include <format>
#include <functional>
#include <iostream>
#include <memory>
#include <print>
#include <stdexcept>
#include <thread>
#include <utility>

class XDisplayWrapper {
 public:
  [[nodiscard]] static std::expected<XDisplayWrapper, std::string>
  create() noexcept {
    if (auto* display = XOpenDisplay(nullptr)) {
      return XDisplayWrapper(display);
    }
    return std::unexpected("Cannot open display");
  }

  XDisplayWrapper(XDisplayWrapper&& other) noexcept
      : display_(std::exchange(other.display_, nullptr)) {}
  XDisplayWrapper& operator=(XDisplayWrapper&& other) noexcept {
    if (this != &other) {
      cleanup();
      display_ = std::exchange(other.display_, nullptr);
    }
    return *this;
  }

  ~XDisplayWrapper() { cleanup(); }

  [[nodiscard]] Display* get() const noexcept { return display_; }
  operator Display*() const noexcept { return display_; }

 private:
  explicit XDisplayWrapper(Display* display) : display_(display) {}
  void cleanup() noexcept {
    if (display_) XCloseDisplay(display_);
  }

  Display* display_;
  XDisplayWrapper(const XDisplayWrapper&) = delete;
  XDisplayWrapper& operator=(const XDisplayWrapper&) = delete;
};

class XWindowWrapper {
 public:
  struct Config {
    int x;
    int y;
    unsigned width;
    unsigned height;
    std::string_view title;
  };

  [[nodiscard]] static std::expected<XWindowWrapper, std::string> create(
      Display* display, int screen, const Config& config) noexcept {
    auto window = XCreateSimpleWindow(
        display, RootWindow(display, screen), config.x, config.y, config.width,
        config.height, 1, BlackPixel(display, screen),
        WhitePixel(display, screen));

    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XStoreName(display, window, config.title.data());
    XMapWindow(display, window);
    return XWindowWrapper(display, window);
  }

  XWindowWrapper(XWindowWrapper&& other) noexcept
      : display_(other.display_), window_(std::exchange(other.window_, 0)) {}

  XWindowWrapper& operator=(XWindowWrapper&& other) noexcept {
    if (this != &other) {
      cleanup();
      display_ = other.display_;
      window_ = std::exchange(other.window_, 0);
    }
    return *this;
  }

  ~XWindowWrapper() { cleanup(); }

  [[nodiscard]] Window get() const noexcept { return window_; }
  operator Window() const noexcept { return window_; }

 private:
  Display* display_;
  Window window_;

  XWindowWrapper(Display* display, Window window)
      : display_(display), window_(window) {}

  void cleanup() noexcept {
    if (window_) XDestroyWindow(display_, window_);
  }

  XWindowWrapper(const XWindowWrapper&) = delete;
  XWindowWrapper& operator=(const XWindowWrapper&) = delete;
};

template <typename T>
concept EventHandlerCallable = requires(T handler, const XEvent& event) {
  { handler(event) } -> std::same_as<bool>;
};

class EventLoop {
 public:
  template <EventHandlerCallable Handler>
  static void run(const XDisplayWrapper& display, std::stop_token stop_token,
                  Handler&& handler) {
    XEvent event;
    while (!stop_token.stop_requested()) {
      XNextEvent(display.get(), &event);
      if (!std::forward<Handler>(handler)(event)) break;
    }
  }
};

int main() {
  auto display_result = XDisplayWrapper::create();
  if (!display_result) {
    std::cerr << std::format("Error: {}\n", display_result.error());
    return 1;
  }
  auto& display = *display_result;

  const int screen = DefaultScreen(display.get());
  auto window_result =
      XWindowWrapper::create(display.get(), screen,
                             XWindowWrapper::Config{.x = 100,
                                                    .y = 100,
                                                    .width = 500,
                                                    .height = 300,
                                                    .title = "My Window"});

  if (!window_result) {
    std::cerr << std::format("Error: {}\n", window_result.error());
    return 1;
  }

  std::jthread event_thread([&display](std::stop_token stop_token) {
    EventLoop::run(display, stop_token, [](const XEvent& event) {
      switch (event.type) {
        case Expose:
          return true;
        case ButtonPress:
          std::println("Button clicked!");
          return true;
        case KeyPress:
          return false;
        default:
          return true;
      }
    });
  });

  event_thread.join();
  return 0;
}
