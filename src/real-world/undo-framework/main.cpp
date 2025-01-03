#include <algorithm>
#include <chrono>
#include <expected>
#include <functional>
#include <memory>
#include <print>
#include <ranges>
#include <source_location>
#include <string>
#include <thread>
#include <utility>

using namespace std::chrono_literals;

enum class CommandError {
  VALIDATION_FAILED,
  EXECUTION_FAILED,
  INVALID_STATE,
  CANCELLED
};

template <typename T>
using CommandResult = std::expected<T, CommandError>;

// Add command validation concept
template <typename T>
concept Commandlike = requires(T t) {
  { t.execute() } -> std::same_as<CommandResult<void>>;
  { t.undo() } -> std::same_as<CommandResult<void>>;
  { t.canExecute() } -> std::same_as<bool>;
  { t.canUndo() } -> std::same_as<bool>;
};

enum class CommandState {
  IDLE,
  EXECUTING,
  EXECUTED,
  UNDOING,
  UNDONE,
  CANCELLED
};

template <typename T>
concept ValidatableCommand = requires(T t) {
  { t.validate() } -> std::same_as<CommandResult<void>>;
  { t.getState() } -> std::same_as<CommandState>;
  { t.cancel() } -> std::same_as<void>;
};

class Command {
 public:
  Command(std::string_view name = "Unnamed Command",
          std::source_location loc = std::source_location::current())
      : name_(name), location_(loc) {}
  virtual ~Command() = default;
  [[nodiscard]] auto operator<=>(const Command&) const = default;

  virtual CommandResult<void> execute() = 0;
  virtual CommandResult<void> undo() = 0;

  [[nodiscard]] virtual CommandResult<void> validate() const {
    return std::expected<void, CommandError>{};
  }

  [[nodiscard]] constexpr auto getStartTime() const noexcept {
    return start_time_;
  }
  [[nodiscard]] constexpr auto getEndTime() const noexcept { return end_time_; }

  [[nodiscard]] virtual bool canExecute() const { return true; }
  [[nodiscard]] virtual bool canUndo() const { return true; }

  [[nodiscard]] constexpr std::string_view getName() const noexcept {
    return name_;
  }
  [[nodiscard]] auto getDuration() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end_time_ -
                                                                 start_time_);
  }
  [[nodiscard]] virtual CommandState getState() const noexcept {
    return state_;
  }
  virtual void cancel() {
    if (state_ == CommandState::EXECUTING) {
      state_ = CommandState::CANCELLED;
    }
  }
  [[nodiscard]] constexpr auto getLocation() const noexcept {
    return location_;
  }

 protected:
  void setState(CommandState state) noexcept { state_ = state; }
  void begin() {
    state_ = CommandState::EXECUTING;
    start_time_ = std::chrono::steady_clock::now();
  }
  void end() {
    state_ = CommandState::EXECUTED;
    end_time_ = std::chrono::steady_clock::now();
  }

 private:
  std::chrono::steady_clock::time_point start_time_{};
  std::chrono::steady_clock::time_point end_time_{};
  std::string name_;
  CommandState state_ = CommandState::IDLE;
  std::source_location location_;
};

class MoveCommand : public Command {
 public:
  MoveCommand(int& x, int& y, int new_x, int new_y,
              std::string_view name = "Unnamed Move Command")
      : Command(name), x_(x), y_(y), new_x_(new_x), new_y_(new_y) {}

  CommandResult<void> validate() const override {
    if (new_x_ < 0 || new_y_ < 0) {
      return std::unexpected(CommandError::VALIDATION_FAILED);
    }
    return {};
  }

  CommandResult<void> execute() override {
    if (auto result = validate(); !result) {
      return result;
    }

    if (!canExecute()) return std::unexpected(CommandError::INVALID_STATE);
    begin();
    old_x_ = x_;
    old_y_ = y_;
    x_ = new_x_;
    y_ = new_y_;
    end();
    return {};
  }

  CommandResult<void> undo() override {
    if (!canUndo()) return std::unexpected(CommandError::INVALID_STATE);
    if (old_x_ && old_y_) {
      x_ = *old_x_;
      y_ = *old_y_;
    }
    return {};
  }

 private:
  int& x_;
  int& y_;
  int new_x_;
  int new_y_;
  std::optional<int> old_x_;
  std::optional<int> old_y_;
};

class ToggleCommand : public Command {
 public:
  ToggleCommand(bool& value, std::string_view name = "Unnamed Toggle Command")
      : Command(name), value_(value) {}

  CommandResult<void> execute() override {
    if (!canExecute()) return std::unexpected(CommandError::INVALID_STATE);
    begin();
    old_state_ = value_;
    std::this_thread::sleep_for(1s);
    value_ = !value_;
    end();
    return {};
  }

  CommandResult<void> undo() override {
    if (!canUndo()) return std::unexpected(CommandError::INVALID_STATE);
    value_ = old_state_;
    return {};
  }

 private:
  bool& value_;
  bool old_state_;
};

class RotationCommand : public Command {
 public:
  RotationCommand(float& angle, float new_angle,
                  std::string_view name = "Unnamed Rotation Command")
      : Command(name), angle_(angle), new_angle_(new_angle) {}

  CommandResult<void> execute() override {
    if (!canExecute()) return std::unexpected(CommandError::INVALID_STATE);

    begin();
    old_angle_ = angle_;
    angle_ = new_angle_;
    end();
    return {};
  }

  CommandResult<void> undo() override {
    if (!canUndo()) return std::unexpected(CommandError::INVALID_STATE);
    if (old_angle_) {
      angle_ = *old_angle_;
    }
    return {};
  }

 private:
  float& angle_;
  float new_angle_;
  std::optional<float> old_angle_;
};

class BulkCommand : public Command {
  std::vector<std::shared_ptr<Command>> commands;

 public:
  BulkCommand(std::string_view name = "Unnamed Bulk Command") : Command(name) {}

  void addCommand(std::shared_ptr<Command> cmd) { commands.push_back(cmd); }

  template <std::ranges::range R>
    requires std::same_as<std::ranges::range_value_t<R>,
                          std::shared_ptr<Command>>
  void addCommands(R&& cmds) {
    commands.insert(commands.end(), std::ranges::begin(cmds),
                    std::ranges::end(cmds));
  }

  CommandResult<void> execute() override {
    if (!canExecute()) return std::unexpected(CommandError::INVALID_STATE);
    if (auto result = validate(); !result) {
      return result;
    }

    begin();
    for (const auto& cmd : commands) {
      if (auto result = cmd->execute(); !result) {
        for (auto it = commands.rbegin(); it != commands.rend() && *it != cmd;
             ++it) {
          (*it)->undo();
        }
        return std::unexpected(result.error());
      }
    }
    end();
    return {};
  }

  CommandResult<void> undo() override {
    if (!canUndo()) return std::unexpected(CommandError::INVALID_STATE);
    for (const auto& cmd : commands | std::views::reverse) {
      cmd->undo();
    }
    return {};
  }

  CommandResult<void> validate() const override {
    return std::ranges::all_of(
               commands,
               [](const auto& cmd) { return cmd->validate().has_value(); })
               ? CommandResult<void>{}
               : std::unexpected(CommandError::VALIDATION_FAILED);
  }
};

class CommandObserver {
 public:
  virtual void onCommandExecuted(const Command& cmd) = 0;
  virtual void onCommandUndone(const Command& cmd) = 0;
  virtual void onCommandRedone(const Command& cmd) = 0;
  virtual ~CommandObserver() = default;
};

class CommandLogger : public CommandObserver {
 public:
  void onCommandExecuted(const Command& cmd) override {
    std::println("Command '{}' executed in {}ms", cmd.getName(),
                 cmd.getDuration().count());
  }
  void onCommandUndone(const Command& cmd) override {
    std::println("Command '{}' undone in {}ms", cmd.getName(),
                 cmd.getDuration().count());
  }
  void onCommandRedone(const Command& cmd) override {
    std::println("Command '{}' redone in {}ms", cmd.getName(),
                 cmd.getDuration().count());
  }
};

class CommandManager {
 public:
  constexpr explicit CommandManager(size_t max_undo_levels = 100)
      : max_undo_levels_(max_undo_levels) {}

  template <Commandlike T>
  CommandResult<void> executeCommand(std::shared_ptr<T> cmd) {
    if (!cmd->canExecute()) {
      return std::unexpected(CommandError::INVALID_STATE);
    }

    if (current_group_) {
      current_group_->addCommand(cmd);
      return CommandResult<void>{};
    }

    auto result = cmd->execute();
    if (!result.has_value()) {
      return result;
    }

    for (const auto& observer : observers_) {
      observer->onCommandExecuted(*cmd);
    }

    history_.push_back(cmd);

    if (cmd->canUndo()) {
      undo_stack_.push_back(std::move(cmd));
      redo_stack_.clear();

      while (undo_stack_.size() > max_undo_levels_) {
        undo_stack_.erase(undo_stack_.begin());
      }
    }
    return result;
  }

  void undo() {
    if (undo_stack_.empty()) return;

    auto& cmd = undo_stack_.back();
    cmd->undo();
    for (const auto& observer : observers_) {
      observer->onCommandUndone(*cmd);
    }

    redo_stack_.push_back(std::move(cmd));
    undo_stack_.pop_back();
  }

  void redo() {
    if (redo_stack_.empty()) return;

    auto& cmd = redo_stack_.back();
    cmd->execute();
    for (const auto& observer : observers_) {
      observer->onCommandRedone(*cmd);
    }

    undo_stack_.push_back(std::move(cmd));
    redo_stack_.pop_back();
  }

  [[nodiscard]] constexpr bool canUndo() const { return !undo_stack_.empty(); }
  [[nodiscard]] constexpr bool canRedo() const { return !redo_stack_.empty(); }
  [[nodiscard]] constexpr std::string_view getUndoName() const noexcept {
    return canUndo() ? undo_stack_.back()->getName() : "Nothing to undo";
  }
  [[nodiscard]] constexpr std::string_view getRedoName() const noexcept {
    return canRedo() ? redo_stack_.back()->getName() : "Nothing to redo";
  }

  void beginCommandGroup(std::string_view name) {
    current_group_ = std::make_shared<BulkCommand>(name);
  }

  void endCommandGroup() {
    if (current_group_) {
      executeCommand(std::move(current_group_));
      current_group_.reset();
    }
  }

  void addObserver(std::shared_ptr<CommandObserver> observer) {
    observers_.push_back(observer);
  }

 private:
  std::vector<std::shared_ptr<Command>> undo_stack_;
  std::vector<std::shared_ptr<Command>> redo_stack_;
  std::vector<std::shared_ptr<Command>> history_;
  const size_t max_undo_levels_;
  std::shared_ptr<BulkCommand> current_group_;
  std::vector<std::shared_ptr<CommandObserver>> observers_;
};

class CommandGroup {
 public:
  explicit CommandGroup(CommandManager& manager, std::string_view name)
      : manager_(manager), name_(name) {
    manager_.beginCommandGroup(name);
  }

  ~CommandGroup() { manager_.endCommandGroup(); }

 private:
  CommandManager& manager_;
  std::string_view name_;
};

// Function to convert CommandError to string
std::string_view commandErrorToString(CommandError error) {
  switch (error) {
    case CommandError::VALIDATION_FAILED:
      return "Validation Failed";
    case CommandError::EXECUTION_FAILED:
      return "Execution Failed";
    case CommandError::INVALID_STATE:
      return "Invalid State";
    case CommandError::CANCELLED:
      return "Cancelled";
    default:
      return "Unknown Error";
  }
}

int main() {
  CommandManager manager(50);  // Limit to 50 undo levels
  manager.addObserver(std::make_shared<CommandLogger>());

  float angle = 0.0f;
  int x = 0;
  int y = 0;

  manager.executeCommand(std::make_shared<MoveCommand>(x, y, 10, 20));
  manager.executeCommand(std::make_shared<RotationCommand>(angle, 45.0f));

  manager.undo();
  manager.redo();

  // Example of command grouping with RAII
  {
    CommandGroup group(manager, "Complex Operation");
    manager.executeCommand(std::make_shared<RotationCommand>(angle, 10.0f));
    manager.executeCommand(std::make_shared<RotationCommand>(angle, 20.0f));
  }

  // Command validation
  auto result =
      manager.executeCommand(std::make_shared<MoveCommand>(x, y, -10, -20));
  if (!result.has_value()) {
    std::println("Command validation failed: {}",
                 commandErrorToString(result.error()));
  }

  // Slow command
  bool toggle = false;
  manager.executeCommand(std::make_shared<ToggleCommand>(toggle));

  // UI integration
  if (manager.canUndo()) {
    std::println("Undo available: {}", manager.getUndoName());
  }

  return 0;
}
