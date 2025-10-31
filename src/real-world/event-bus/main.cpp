#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <format>
#include <functional>
#include <memory>
#include <mutex>
#include <print>
#include <ranges>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <vector>

/**
 * @brief Type-safe event bus for decoupled communication between components
 *
 * The EventBus provides a publish-subscribe pattern implementation that allows
 * components to communicate without direct dependencies. Events are type-safe
 * and subscribers receive only the events they're interested in.
 */

/**
 * @brief Unique identifier for event subscriptions
 */
using SubscriptionId = size_t;

/**
 * @brief Concept for event types - must be copyable
 */
template <typename T>
concept Event = requires { requires std::is_copy_constructible_v<T>; };

// Forward declaration
template <Event EventType>
class EventSubscription;

/**
 * @brief Base class for type-erased event handlers
 */
class EventHandlerBase {
 public:
  EventHandlerBase() = default;
  virtual ~EventHandlerBase() = default;
  EventHandlerBase(const EventHandlerBase&) = default;
  EventHandlerBase& operator=(const EventHandlerBase&) = default;
  EventHandlerBase(EventHandlerBase&&) = default;
  EventHandlerBase& operator=(EventHandlerBase&&) = default;
  [[nodiscard]] virtual SubscriptionId getId() const = 0;
};

/**
 * @brief Type-safe event handler wrapper
 */
template <Event EventType>
class EventHandler : public EventHandlerBase {
 public:
  using HandlerFunc = std::function<void(const EventType&)>;

  explicit EventHandler(SubscriptionId id, HandlerFunc handler)
      : id_(id), handler_(std::move(handler)) {}

  void invoke(const EventType& event) const { handler_(event); }

  [[nodiscard]] SubscriptionId getId() const override { return id_; }

 private:
  SubscriptionId id_;
  HandlerFunc handler_;
};

/**
 * @brief Main event bus class for pub/sub communication
 */
class EventBus {
 public:
  /**
   * @brief Subscribe to events of a specific type
   * @tparam EventType The type of event to subscribe to
   * @param handler Function to call when event is published
   * @return RAII subscription handle
   */
  template <Event EventType>
  [[nodiscard]] std::shared_ptr<EventSubscription<EventType>> subscribe(
      typename EventHandler<EventType>::HandlerFunc handler) {
    std::lock_guard lock(mutex_);

    const auto id = next_id_++;
    const auto type_index = std::type_index(typeid(EventType));

    auto event_handler =
        std::make_shared<EventHandler<EventType>>(id, std::move(handler));

    handlers_[type_index].push_back(event_handler);

    // Create and return subscription handle
    return std::make_shared<EventSubscription<EventType>>(this, id, type_index,
                                                          event_handler);
  }

  /**
   * @brief Publish an event to all subscribers
   * @tparam EventType The type of event to publish
   * @param event The event to publish
   */
  template <Event EventType>
  void publish(const EventType& event) {
    std::vector<std::shared_ptr<EventHandler<EventType>>> handlers_copy;

    {
      std::lock_guard lock(mutex_);
      const auto type_index = std::type_index(typeid(EventType));

      auto it = handlers_.find(type_index);
      if (it == handlers_.end()) {
        return;
      }

      // Copy handlers to avoid holding lock during event dispatch
      handlers_copy.reserve(it->second.size());
      for (const auto& handler : it->second) {
        if (auto typed_handler =
                std::dynamic_pointer_cast<EventHandler<EventType>>(handler)) {
          handlers_copy.push_back(typed_handler);
        }
      }
    }

    // Dispatch events without holding the lock
    for (const auto& handler : handlers_copy) {
      try {
        handler->invoke(event);
      } catch (const std::exception& e) {
        std::println("Exception in event handler: {}", e.what());
      } catch (...) {
        std::println("Unknown exception in event handler");
      }
    }
  }

  /**
   * @brief Unsubscribe a specific handler
   * @param id Subscription ID to unsubscribe
   * @param type_index Type index of the event
   */
  void unsubscribe(SubscriptionId id, std::type_index type_index) {
    std::lock_guard lock(mutex_);

    auto it = handlers_.find(type_index);
    if (it == handlers_.end()) {
      return;
    }

    auto& handler_list = it->second;
    std::erase_if(handler_list,
                  [id](const auto& handler) { return handler->getId() == id; });

    if (handler_list.empty()) {
      handlers_.erase(it);
    }
  }

  /**
   * @brief Get the number of subscribers for a specific event type
   * @tparam EventType The event type to query
   * @return Number of subscribers
   */
  template <Event EventType>
  [[nodiscard]] size_t getSubscriberCount() const {
    std::lock_guard lock(mutex_);
    const auto type_index = std::type_index(typeid(EventType));

    auto it = handlers_.find(type_index);
    return it != handlers_.end() ? it->second.size() : 0;
  }

  /**
   * @brief Clear all subscriptions
   */
  void clear() {
    std::lock_guard lock(mutex_);
    handlers_.clear();
  }

 private:
  mutable std::mutex mutex_;
  std::unordered_map<std::type_index,
                     std::vector<std::shared_ptr<EventHandlerBase>>>
      handlers_;
  SubscriptionId next_id_ = 0;
};

/**
 * @brief RAII subscription handle that automatically unsubscribes
 */
template <Event EventType>
class EventSubscription {
 public:
  EventSubscription(EventBus* bus, SubscriptionId id,
                    std::type_index type_index,
                    std::shared_ptr<EventHandler<EventType>> handler)
      : bus_(bus),
        id_(id),
        type_index_(type_index),
        handler_(std::move(handler)) {}

  ~EventSubscription() {
    if (bus_ && !unsubscribed_) {
      unsubscribe();
    }
  }

  // Non-copyable, movable
  EventSubscription(const EventSubscription&) = delete;
  EventSubscription& operator=(const EventSubscription&) = delete;
  EventSubscription(EventSubscription&& other) noexcept
      : bus_(other.bus_),
        id_(other.id_),
        type_index_(other.type_index_),
        handler_(std::move(other.handler_)),
        unsubscribed_(other.unsubscribed_) {
    other.bus_ = nullptr;
  }
  EventSubscription& operator=(EventSubscription&& other) noexcept {
    if (this != &other) {
      if (bus_ && !unsubscribed_) {
        unsubscribe();
      }
      bus_ = other.bus_;
      id_ = other.id_;
      type_index_ = other.type_index_;
      handler_ = std::move(other.handler_);
      unsubscribed_ = other.unsubscribed_;
      other.bus_ = nullptr;
    }
    return *this;
  }

  /**
   * @brief Manually unsubscribe
   */
  void unsubscribe() {
    if (bus_ && !unsubscribed_) {
      bus_->unsubscribe(id_, type_index_);
      unsubscribed_ = true;
    }
  }

  [[nodiscard]] SubscriptionId getId() const { return id_; }
  [[nodiscard]] bool isActive() const { return !unsubscribed_; }

 private:
  EventBus* bus_;
  SubscriptionId id_;
  std::type_index type_index_;
  std::shared_ptr<EventHandler<EventType>> handler_;
  bool unsubscribed_ = false;
};

// ============================================================================
// Example Events
// ============================================================================

/**
 * @brief User authentication event
 */
struct UserLoggedIn {
  std::string username;
  std::string user_id;
  bool is_admin;
};

/**
 * @brief User logout event
 */
struct UserLoggedOut {
  std::string username;
  std::string reason;
};

/**
 * @brief Data update event
 */
struct DataUpdated {
  std::string entity_type;
  std::string entity_id;
  std::unordered_map<std::string, std::string> changes;
};

/**
 * @brief Network status event
 */
struct NetworkStatusChanged {
  enum class Status : std::uint8_t { Connected, Disconnected, Reconnecting };

  Status status;
  std::string message;

  [[nodiscard]] std::string_view getStatusString() const {
    switch (status) {
      case Status::Connected:
        return "Connected";
      case Status::Disconnected:
        return "Disconnected";
      case Status::Reconnecting:
        return "Reconnecting";
      default:
        return "Unknown";
    }
  }
};

/**
 * @brief Error event
 */
struct ErrorOccurred {
  enum class Severity : std::uint8_t { Info, Warning, Error, Critical };

  Severity severity;
  std::string message;
  std::string component;

  [[nodiscard]] std::string_view getSeverityString() const {
    switch (severity) {
      case Severity::Info:
        return "INFO";
      case Severity::Warning:
        return "WARNING";
      case Severity::Error:
        return "ERROR";
      case Severity::Critical:
        return "CRITICAL";
      default:
        return "UNKNOWN";
    }
  }
};

// ============================================================================
// Example Subscribers (Components)
// ============================================================================

/**
 * @brief Logger component that subscribes to multiple event types
 */
class Logger {
 public:
  explicit Logger(EventBus& bus)
      : login_subscription_(bus.subscribe<UserLoggedIn>([](const auto& event) {
          std::println("[Logger] User logged in: {} (ID: {}, Admin: {})",
                       event.username, event.user_id,
                       event.is_admin ? "Yes" : "No");
        })),
        logout_subscription_(
            bus.subscribe<UserLoggedOut>([](const auto& event) {
              std::println("[Logger] User logged out: {} (Reason: {})",
                           event.username, event.reason);
            })),
        error_subscription_(bus.subscribe<ErrorOccurred>([](const auto& event) {
          std::println("[Logger] {} in {}: {}", event.getSeverityString(),
                       event.component, event.message);
        })) {}

 private:
  std::shared_ptr<EventSubscription<UserLoggedIn>> login_subscription_;
  std::shared_ptr<EventSubscription<UserLoggedOut>> logout_subscription_;
  std::shared_ptr<EventSubscription<ErrorOccurred>> error_subscription_;
};

/**
 * @brief Analytics component that tracks user behavior
 */
class Analytics {
 public:
  explicit Analytics(EventBus& bus) {
    login_subscription_ =
        bus.subscribe<UserLoggedIn>([this](const UserLoggedIn& event) {
          login_count_++;
          std::println("[Analytics] Login tracked: {} (Total logins: {})",
                       event.username, login_count_);
        });

    logout_subscription_ =
        bus.subscribe<UserLoggedOut>([this](const UserLoggedOut& event) {
          logout_count_++;
          std::println("[Analytics] Logout tracked: {} (Total logouts: {})",
                       event.username, logout_count_);
        });
  }

  [[nodiscard]] size_t getLoginCount() const { return login_count_; }
  [[nodiscard]] size_t getLogoutCount() const { return logout_count_; }

  void printStats() const {
    std::println("[Analytics] Statistics:");
    std::println("  Total logins:  {}", login_count_);
    std::println("  Total logouts: {}", logout_count_);
  }

 private:
  std::shared_ptr<EventSubscription<UserLoggedIn>> login_subscription_;
  std::shared_ptr<EventSubscription<UserLoggedOut>> logout_subscription_;
  size_t login_count_ = 0;
  size_t logout_count_ = 0;
};

/**
 * @brief UI component that updates based on events
 */
class UIComponent {
 public:
  explicit UIComponent(EventBus& bus, std::string name)
      : name_(std::move(name)) {
    network_subscription_ = bus.subscribe<NetworkStatusChanged>(
        [this](const NetworkStatusChanged& event) {
          std::println("[UI:{}] Network status: {} - {}", name_,
                       event.getStatusString(), event.message);
        });

    data_subscription_ =
        bus.subscribe<DataUpdated>([this](const DataUpdated& event) {
          std::println("[UI:{}] Data updated: {} (ID: {})", name_,
                       event.entity_type, event.entity_id);
        });
  }

 private:
  std::string name_;
  std::shared_ptr<EventSubscription<NetworkStatusChanged>>
      network_subscription_;
  std::shared_ptr<EventSubscription<DataUpdated>> data_subscription_;
};

/**
 * @brief Error handler component
 */
class ErrorHandler {
 public:
  explicit ErrorHandler(EventBus& bus) {
    error_subscription_ =
        bus.subscribe<ErrorOccurred>([this](const ErrorOccurred& event) {
          if (event.severity == ErrorOccurred::Severity::Critical) {
            std::println("[ErrorHandler] CRITICAL ERROR DETECTED!");
            std::println("[ErrorHandler] Component: {}", event.component);
            std::println("[ErrorHandler] Message: {}", event.message);
            std::println("[ErrorHandler] Initiating recovery procedures...");
          } else if (event.severity == ErrorOccurred::Severity::Error) {
            std::println("[ErrorHandler] ERROR DETECTED!");
            std::println("[ErrorHandler] Component: {}", event.component);
            std::println("[ErrorHandler] Message: {}", event.message);
            std::println("[ErrorHandler] Initiating recovery procedures...");
          } else if (event.severity == ErrorOccurred::Severity::Warning) {
            std::println("[ErrorHandler] WARNING DETECTED!");
            std::println("[ErrorHandler] Component: {}", event.component);
            std::println("[ErrorHandler] Message: {}", event.message);
            std::println("[ErrorHandler] Initiating recovery procedures...");
          } else if (event.severity == ErrorOccurred::Severity::Info) {
            std::println("[ErrorHandler] INFO DETECTED!");
            std::println("[ErrorHandler] Component: {}", event.component);
            std::println("[ErrorHandler] Message: {}", event.message);
            std::println("[ErrorHandler] Initiating recovery procedures...");
          }
        });
  }

 private:
  std::shared_ptr<EventSubscription<ErrorOccurred>> error_subscription_;
};

// ============================================================================
// Demonstration
// ============================================================================

/**
 * @brief Demonstrate basic pub/sub functionality
 */
void demonstrateBasicPubSub(EventBus& bus) {
  std::println("\n=== Basic Pub/Sub Demo ===\n");

  // Create components
  [[maybe_unused]] Logger logger(bus);
  Analytics analytics(bus);

  std::println("Publishing login event...");
  bus.publish(UserLoggedIn{
      .username = "alice", .user_id = "user_001", .is_admin = false});

  std::println("\nPublishing another login event...");
  bus.publish(
      UserLoggedIn{.username = "bob", .user_id = "user_002", .is_admin = true});

  std::println("\nPublishing logout event...");
  bus.publish(UserLoggedOut{.username = "alice", .reason = "User logged out"});

  std::println("");
  analytics.printStats();
}

/**
 * @brief Demonstrate multiple subscribers
 */
void demonstrateMultipleSubscribers(EventBus& bus) {
  std::println("\n=== Multiple Subscribers Demo ===\n");

  [[maybe_unused]] UIComponent ui1(bus, "Dashboard");
  [[maybe_unused]] UIComponent ui2(bus, "StatusBar");
  [[maybe_unused]] UIComponent ui3(bus, "Notification");

  std::println("Publishing network status event...");
  bus.publish(
      NetworkStatusChanged{.status = NetworkStatusChanged::Status::Connected,
                           .message = "Connection established"});

  std::println("\nPublishing data update event...");
  bus.publish(DataUpdated{
      .entity_type = "User",
      .entity_id = "user_001",
      .changes = {{"name", "Alice"}, {"email", "alice@example.com"}}});
}

/**
 * @brief Demonstrate RAII subscription management
 */
void demonstrateSubscriptionLifetime(EventBus& bus) {
  std::println("\n=== Subscription Lifetime Demo ===\n");

  std::println("Subscriber count before: {}",
               bus.getSubscriberCount<ErrorOccurred>());

  {
    [[maybe_unused]] ErrorHandler handler(bus);
    std::println("Subscriber count with handler: {}",
                 bus.getSubscriberCount<ErrorOccurred>());

    std::println("\nPublishing critical error...");
    bus.publish(ErrorOccurred{.severity = ErrorOccurred::Severity::Critical,
                              .message = "Database connection lost",
                              .component = "DatabaseService"});

    std::println("\nPublishing warning...");
    bus.publish(ErrorOccurred{.severity = ErrorOccurred::Severity::Warning,
                              .message = "High memory usage detected",
                              .component = "MemoryMonitor"});
  }  // ErrorHandler goes out of scope, subscription automatically removed

  std::println("\nSubscriber count after handler destroyed: {}",
               bus.getSubscriberCount<ErrorOccurred>());

  std::println("\nPublishing error (should not be handled)...");
  bus.publish(ErrorOccurred{.severity = ErrorOccurred::Severity::Error,
                            .message = "This error should not be logged",
                            .component = "TestComponent"});
}

/**
 * @brief Demonstrate lambda subscriptions
 */
void demonstrateLambdaSubscriptions(EventBus& bus) {
  std::println("\n=== Lambda Subscriptions Demo ===\n");

  auto sub1 = bus.subscribe<DataUpdated>([](const DataUpdated& event) {
    std::println("[Lambda1] Data updated: {}", event.entity_type);
  });

  auto sub2 = bus.subscribe<DataUpdated>([](const DataUpdated& event) {
    std::println("[Lambda2] Changes detected for entity: {}", event.entity_id);
  });

  std::println("Publishing data update with two lambda subscribers...");
  bus.publish(DataUpdated{.entity_type = "Product",
                          .entity_id = "prod_123",
                          .changes = {{"price", "29.99"}}});

  std::println("\nUnsubscribing first lambda...");
  sub1->unsubscribe();

  std::println(
      "\nPublishing another data update (only second lambda should "
      "receive)...");
  bus.publish(DataUpdated{.entity_type = "Product",
                          .entity_id = "prod_456",
                          .changes = {{"stock", "100"}}});
}

/**
 * @brief Demonstrate event filtering pattern
 */
void demonstrateEventFiltering(EventBus& bus) {
  std::println("\n=== Event Filtering Demo ===\n");

  // Subscribe with filtering logic
  auto sub = bus.subscribe<ErrorOccurred>([](const ErrorOccurred& event) {
    // Only handle critical and error severity
    if (event.severity == ErrorOccurred::Severity::Critical ||
        event.severity == ErrorOccurred::Severity::Error) {
      std::println("[FilteredHandler] Handling {} severity event from {}",
                   event.getSeverityString(), event.component);
    }
  });

  std::println("Publishing info event (will be ignored by filter)...");
  bus.publish(ErrorOccurred{.severity = ErrorOccurred::Severity::Info,
                            .message = "Application started",
                            .component = "MainApp"});

  std::println("\nPublishing error event (will be handled)...");
  bus.publish(ErrorOccurred{.severity = ErrorOccurred::Severity::Error,
                            .message = "Failed to load resource",
                            .component = "ResourceLoader"});

  std::println("\nPublishing warning event (will be ignored by filter)...");
  bus.publish(ErrorOccurred{.severity = ErrorOccurred::Severity::Warning,
                            .message = "Cache nearly full",
                            .component = "CacheManager"});
}

/**
 * @brief Main demonstration function
 */
void demo() {
  EventBus bus;

  demonstrateBasicPubSub(bus);
  bus.clear();

  demonstrateMultipleSubscribers(bus);
  bus.clear();

  demonstrateSubscriptionLifetime(bus);
  bus.clear();

  demonstrateLambdaSubscriptions(bus);
  bus.clear();

  demonstrateEventFiltering(bus);
  bus.clear();

  std::println("\n=== Demo Complete ===\n");
}

int main() {
  std::println("Event Bus Demonstration");
  std::println("=======================");

  try {
    demo();
  } catch (const std::exception& e) {
    std::println("Error during demonstration: {}", e.what());
    return 1;
  }

  return 0;
}
