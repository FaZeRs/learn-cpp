#include <algorithm>
#include <concepts>
#include <expected>
#include <format>
#include <functional>
#include <iostream>
#include <optional>
#include <print>
#include <ranges>
#include <source_location>
#include <span>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

using namespace std::string_view_literals;

enum class ArgType {
  Flag,       // --verbose
  KeyValue,   // --output=file.txt
  Positional  // command arg1 arg2
};

struct ArgDefinition {
  std::string_view name;
  std::string_view short_name;
  std::string_view description;
  ArgType type;
  bool required = false;
  std::variant<std::monostate, bool, std::string_view> default_value =
      std::monostate{};
  std::function<bool(std::string_view)> validator = nullptr;
};

struct ParsedArg {
  std::string_view name;
  std::variant<std::monostate, bool, std::string_view> value;
};

struct ParseResult {
  std::unordered_map<std::string_view, ParsedArg> named_args;

  template <typename T = std::string_view>
  [[nodiscard]] std::expected<T, std::string> get(std::string_view name) const {
    if (auto it = named_args.find(name); it != named_args.end()) {
      if (auto* value = std::get_if<T>(&it->second.value)) {
        return *value;
      }
      return std::unexpected(
          std::format("Type mismatch for argument: {}", name));
    }
    return std::unexpected(std::format("Argument not found: {}", name));
  }

  template <typename T = std::string_view>
  T get_or(std::string_view name, T default_value) const {
    return get<T>(name).value_or(default_value);
  }
};

struct ParseError {
  enum class Code {
    NoArguments,
    UnknownArgument,
    MissingRequired,
    InvalidValue
  };

  Code code;
  std::string_view context{};
  std::source_location location = std::source_location::current();

  [[nodiscard]] std::string message() const {
    switch (code) {
      case Code::NoArguments:
        return "No arguments provided";
      case Code::UnknownArgument:
        return std::format("Unknown argument: {}", context);
      case Code::MissingRequired:
        return std::format("Missing required argument: {}", context);
      case Code::InvalidValue:
        return std::format("Invalid value for argument {}", context);
    }
    return "Unknown error";
  }
};

template <typename T>
concept StringLike = std::convertible_to<T, std::string_view>;

class ArgumentParser {
 public:
  explicit ArgumentParser(std::string_view description)
      : m_description(description) {}

  template <StringLike Name, StringLike Desc>
  class ArgBuilder {
    ArgumentParser& parser;
    ArgDefinition def;

   public:
    ArgBuilder(ArgumentParser& p, Name&& name, Desc&& desc, ArgType type)
        : parser(p),
          def{std::string_view(std::forward<Name>(name)), "",
              std::string_view(std::forward<Desc>(desc)), type} {}

    ArgBuilder& shortName(std::string_view short_name) {
      def.short_name = short_name;
      return *this;
    }

    ArgBuilder& required(bool req = true) {
      def.required = req;
      return *this;
    }

    ArgBuilder& defaultValue(auto value) {
      def.default_value = value;
      return *this;
    }

    ArgBuilder& validator(auto&& val) {
      def.validator = std::forward<decltype(val)>(val);
      return *this;
    }

    ArgumentParser& build() {
      parser.m_definitions.push_back(std::move(def));
      return parser;
    }
  };

  template <StringLike Name, StringLike Desc>
  [[nodiscard]] ArgBuilder<Name, Desc> arg(Name&& name, Desc&& desc,
                                           ArgType type) {
    return ArgBuilder(*this, std::forward<Name>(name), std::forward<Desc>(desc),
                      type);
  }

  [[nodiscard]] std::expected<ParseResult, ParseError> parse(
      std::span<const char*> args) const {
    if (args.empty()) {
      return std::unexpected(ParseError{ParseError::Code::NoArguments});
    }

    ParseResult result;

    auto positional_defs =
        m_definitions | std::views::filter([](const auto& def) {
          return def.type == ArgType::Positional;
        });

    for (const auto& arg : args.subspan(1)) {
      auto arg_view = std::string_view(arg);

      if (arg_view.starts_with("--"sv)) {
        if (auto err = parseNamedArg(arg_view.substr(2), result.named_args)) {
          return std::unexpected(*err);
        }
      } else {
        auto pos_def =
            std::ranges::find_if(positional_defs, [&result](const auto& def) {
              return !result.named_args.contains(def.name);
            });

        if (pos_def != std::ranges::end(positional_defs)) {
          result.named_args[pos_def->name] = ParsedArg{
              pos_def->name,
              std::variant<std::monostate, bool, std::string_view>{arg_view}};
        }
      }
    }

    if (auto err = validateRequiredArgs(result.named_args)) {
      return std::unexpected(*err);
    }
    return result;
  }

  void printHelp() const {
    std::println("{}", m_description);
    std::println("Arguments:");
    auto sorted_defs = m_definitions | std::ranges::views::all;
    for (const auto type :
         {ArgType::Flag, ArgType::KeyValue, ArgType::Positional}) {
      auto group = sorted_defs | std::views::filter([type](const auto& def) {
                     return def.type == type;
                   });

      if (!std::ranges::empty(group)) {
        std::println(" {}", [type] {
          switch (type) {
            case ArgType::Flag:
              return "Flags:";
            case ArgType::KeyValue:
              return "Options:";
            case ArgType::Positional:
              return "Positional Arguments:";
            default:
              return "Other:";
          }
        }());

        for (const auto& def : group) {
          auto name_str = [&] {
            if (def.type == ArgType::Positional) {
              return std::format("{}", def.name);
            }
            if (def.short_name.empty()) {
              return std::format("--{}", def.name);
            }
            return std::format("-{}, --{}", def.short_name, def.name);
          }();

          auto default_str = std::visit(
              [](const auto& val) -> std::string {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::monostate>) {
                  return "";
                } else if constexpr (std::is_same_v<T, bool>) {
                  return val ? " [default: true]" : " [default: false]";
                } else {
                  return std::format(" [default: {}]", val);
                }
              },
              def.default_value);

          std::println("    {:<20} {}{}{}", name_str, def.description,
                       def.required ? " (required)" : "", default_str);
        }
      }
    }
    std::println();
  }

 private:
  std::string_view m_description;
  std::vector<ArgDefinition> m_definitions;

  [[nodiscard]] std::optional<ParseError> parseNamedArg(
      std::string_view arg,
      std::unordered_map<std::string_view, ParsedArg>& parsed_args) const {
    auto [name, value] = [&arg]()
        -> std::pair<std::string_view, std::optional<std::string_view>> {
      if (auto pos = arg.find('='); pos != std::string_view::npos) {
        return {arg.substr(0, pos), arg.substr(pos + 1)};
      }
      return {arg, std::nullopt};
    }();

    auto arg_def = std::ranges::find_if(
        m_definitions, [&name](const auto& def) { return def.name == name; });

    if (arg_def == m_definitions.end()) {
      return ParseError{ParseError::Code::UnknownArgument, name};
    }

    if (arg_def->validator && value.has_value()) {
      if (!arg_def->validator(*value)) {
        return ParseError{ParseError::Code::InvalidValue, name};
      }
    }

    parsed_args[name] = ParsedArg{
        name, arg_def->type == ArgType::Flag
                  ? std::variant<std::monostate, bool, std::string_view>{true}
                  : std::variant<std::monostate, bool, std::string_view>{
                        value.value_or("")}};
    return std::nullopt;
  }

  [[nodiscard]] std::optional<ParseError> validateRequiredArgs(
      const std::unordered_map<std::string_view, ParsedArg>& parsed_args)
      const {
    auto required_args =
        m_definitions | std::views::filter([](const ArgDefinition& def) {
          return def.required;
        });

    for (const auto& def : required_args) {
      if (!parsed_args.contains(def.name)) {
        return ParseError{ParseError::Code::MissingRequired, def.name};
      }
    }
    return std::nullopt;
  }

  static constexpr std::string_view getTypeString(ArgType type) {
    switch (type) {
      case ArgType::Flag:
        return "[flag]";
      case ArgType::KeyValue:
        return "[key=value]";
      case ArgType::Positional:
        return "[positional]";
      default:
        return "[unknown]";
    }
  }
};

int main(int argc, const char* argv[]) {
  ArgumentParser parser("Modern C++ Command Line Parser Example");
  parser.arg("verbose"sv, "Enable verbose output"sv, ArgType::Flag)
      .shortName("v")
      .build();
  parser.arg("output"sv, "Output file path"sv, ArgType::KeyValue)
      .shortName("o")
      .required()
      .validator([](std::string_view value) { return value.ends_with(".txt"); })
      .build();
  parser.arg("input"sv, "Input file path"sv, ArgType::Positional)
      .required()
      .build();

  if (argc == 1) {
    parser.printHelp();
    return 0;
  }

  auto result = parser.parse({argv, static_cast<size_t>(argc)});

  if (!result) {
    const auto& error = result.error();
    std::println(stderr, "Error: {}", error.message());
    std::println(stderr, "Location: {}:{}", error.location.file_name(),
                 error.location.line());
    return 1;
  }

  if (auto verbose = result->get<bool>("verbose"); verbose && *verbose) {
    std::println("Verbose mode enabled");
  }

  if (auto output = result->get<std::string_view>("output"); output) {
    std::println("Output file: {}", *output);
  }

  if (auto input = result->get<std::string_view>("input"); input) {
    std::println("Input file: {}", *input);
  }

  return 0;
}
