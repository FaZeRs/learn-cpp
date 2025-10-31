#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cctype>
#include <cstdint>
#include <cstring>
#include <expected>
#include <format>
#include <map>
#include <memory>
#include <print>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

// HTTP Method enumeration
enum class HttpMethod : std::uint8_t { GET, POST, PUT, DELETE, HEAD };

// Convert HTTP method to string
constexpr std::string_view method_to_string(HttpMethod method) {
  switch (method) {
    case HttpMethod::GET:
      return "GET";
    case HttpMethod::POST:
      return "POST";
    case HttpMethod::PUT:
      return "PUT";
    case HttpMethod::DELETE:
      return "DELETE";
    case HttpMethod::HEAD:
      return "HEAD";
  }
  return "GET";
}

// URL parser
struct URL {
  std::string protocol;
  std::string host;
  std::uint16_t port{80};
  std::string path;

  static std::expected<URL, std::string> parse(std::string_view url) {
    URL result;

    // Parse protocol
    auto protocol_end = url.find("://");
    if (protocol_end == std::string_view::npos) {
      return std::unexpected("Invalid URL: missing protocol");
    }

    result.protocol = url.substr(0, protocol_end);
    url.remove_prefix(protocol_end + 3);

    // Parse host and port
    auto path_start = url.find('/');
    auto host_part = (path_start != std::string_view::npos)
                         ? url.substr(0, path_start)
                         : url;

    auto port_start = host_part.find(':');
    if (port_start != std::string_view::npos) {
      result.host = host_part.substr(0, port_start);
      auto port_str = host_part.substr(port_start + 1);
      try {
        result.port =
            static_cast<std::uint16_t>(std::stoi(std::string(port_str)));
      } catch (...) {
        return std::unexpected("Invalid port number");
      }
    } else {
      result.host = host_part;
      result.port = (result.protocol == "https") ? 443 : 80;
    }

    // Parse path
    if (path_start != std::string_view::npos) {
      result.path = url.substr(path_start);
    } else {
      result.path = "/";
    }

    return result;
  }
};

// HTTP Headers
using Headers = std::map<std::string, std::string>;

// HTTP Response
struct HttpResponse {
  int status_code{0};
  std::string status_message;
  Headers headers;
  std::string body;

  static std::expected<HttpResponse, std::string> parse(
      const std::string& raw_response) {
    HttpResponse response;

    std::istringstream stream(raw_response);
    std::string line;

    // Parse status line
    if (!std::getline(stream, line)) {
      return std::unexpected("Empty response");
    }

    // Remove carriage return if present
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }

    // Parse HTTP version, status code, and message
    std::istringstream status_stream(line);
    std::string http_version;
    status_stream >> http_version >> response.status_code;
    std::getline(status_stream, response.status_message);

    // Trim leading space from status message
    if (!response.status_message.empty() &&
        response.status_message.front() == ' ') {
      response.status_message.erase(0, 1);
    }

    // Parse headers
    while (std::getline(stream, line)) {
      if (!line.empty() && line.back() == '\r') {
        line.pop_back();
      }

      if (line.empty()) {
        break;  // End of headers
      }

      auto colon = line.find(':');
      if (colon != std::string::npos) {
        auto key = line.substr(0, colon);
        auto value = line.substr(colon + 1);

        // Trim whitespace from value
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        response.headers[key] = value;
      }
    }

    // Rest is body
    std::ostringstream body_stream;
    body_stream << stream.rdbuf();
    response.body = body_stream.str();

    return response;
  }

  [[nodiscard]] bool is_success() const {
    return status_code >= 200 && status_code < 300;
  }

  void print() const {
    std::println("Status: {} {}", status_code, status_message);
    std::println("Headers:");
    for (const auto& [key, value] : headers) {
      std::println("  {}: {}", key, value);
    }
    std::println("\nBody ({} bytes):", body.size());
    if (body.size() <= 500) {
      std::println("{}", body);
    } else {
      std::println("{}...", body.substr(0, 500));
    }
  }
};

// RAII wrapper for socket file descriptor
class Socket {
 public:
  Socket() : fd_(-1) {}

  explicit Socket(int fd) : fd_(fd) {}

  ~Socket() { close(); }

  Socket(const Socket&) = delete;
  Socket& operator=(const Socket&) = delete;

  Socket(Socket&& other) noexcept : fd_(other.fd_) { other.fd_ = -1; }

  Socket& operator=(Socket&& other) noexcept {
    if (this != &other) {
      close();
      fd_ = other.fd_;
      other.fd_ = -1;
    }
    return *this;
  }

  [[nodiscard]] bool is_valid() const { return fd_ >= 0; }

  [[nodiscard]] int get() const { return fd_; }

  void close() {
    if (fd_ >= 0) {
      ::close(fd_);
      fd_ = -1;
    }
  }

 private:
  int fd_;
};

// HTTP Client
class HttpClient {
 public:
  HttpClient() = default;

  static std::expected<HttpResponse, std::string> request(
      HttpMethod method, std::string_view url, const Headers& headers = {},
      std::string_view body = "") {
    // Parse URL
    auto parsed_url = URL::parse(url);
    if (!parsed_url) {
      return std::unexpected(parsed_url.error());
    }

    // Only support HTTP for now
    if (parsed_url->protocol != "http") {
      return std::unexpected("Only HTTP protocol is supported (not HTTPS)");
    }

    // Connect to server
    auto socket = connect_to_host(parsed_url->host, parsed_url->port);
    if (!socket) {
      return std::unexpected(socket.error());
    }

    // Build HTTP request
    std::string request_str = build_request(method, *parsed_url, headers, body);

    // Send request
    if (!send_data(socket->get(), request_str)) {
      return std::unexpected("Failed to send request");
    }

    // Receive response
    auto response_str = receive_response(socket->get());
    if (!response_str) {
      return std::unexpected(response_str.error());
    }

    // Parse response
    return HttpResponse::parse(*response_str);
  }

  static std::expected<HttpResponse, std::string> get(
      std::string_view url, const Headers& headers = {}) {
    return request(HttpMethod::GET, url, headers);
  }

  static std::expected<HttpResponse, std::string> post(
      std::string_view url, std::string_view body,
      const Headers& headers = {}) {
    return request(HttpMethod::POST, url, headers, body);
  }

 private:
  static std::expected<Socket, std::string> connect_to_host(
      const std::string& host, std::uint16_t port) {
    // Resolve hostname
    struct addrinfo hints{};
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM;  // TCP

    struct addrinfo* result = nullptr;
    int status = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints,
                             &result);
    if (status != 0) {
      return std::unexpected(
          std::format("Failed to resolve host: {}", gai_strerror(status)));
    }

    // RAII wrapper for addrinfo
    struct AddrInfoDeleter {
      void operator()(struct addrinfo* ptr) const {
        if (ptr != nullptr) {
          freeaddrinfo(ptr);
        }
      }
    };
    std::unique_ptr<struct addrinfo, AddrInfoDeleter> addr_guard(result);

    // Try to connect
    for (struct addrinfo* rp = result; rp != nullptr; rp = rp->ai_next) {
      int sock_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
      if (sock_fd < 0) {
        continue;
      }

      Socket sock(sock_fd);

      if (connect(sock.get(), rp->ai_addr, rp->ai_addrlen) == 0) {
        return sock;
      }
    }

    return std::unexpected("Failed to connect to host");
  }

  static std::string build_request(HttpMethod method, const URL& url,
                                   const Headers& headers,
                                   std::string_view body) {
    std::ostringstream request_stream;

    // Request line
    request_stream << method_to_string(method) << " " << url.path
                   << " HTTP/1.1\r\n";

    // Host header (required for HTTP/1.1)
    request_stream << "Host: " << url.host << "\r\n";

    // User headers
    for (const auto& [key, value] : headers) {
      request_stream << key << ": " << value << "\r\n";
    }

    // Content-Length if body is present
    if (!body.empty()) {
      request_stream << "Content-Length: " << body.size() << "\r\n";
    }

    // Connection: close to simplify response handling
    request_stream << "Connection: close\r\n";

    // End of headers
    request_stream << "\r\n";

    // Body
    if (!body.empty()) {
      request_stream << body;
    }

    return request_stream.str();
  }

  static bool send_data(int sock_fd, const std::string& data) {
    size_t total_sent = 0;
    const auto data_size = data.size();

    while (total_sent < data_size) {
      ssize_t sent =
          // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
          send(sock_fd, data.data() + total_sent, data_size - total_sent, 0);
      if (sent < 0) {
        return false;
      }
      total_sent += static_cast<size_t>(sent);
    }
    return true;
  }

  static std::expected<std::string, std::string> receive_response(int sock_fd) {
    std::string response;
    std::vector<char> buffer(4096);

    while (true) {
      ssize_t received = recv(sock_fd, buffer.data(), buffer.size(), 0);
      if (received < 0) {
        return std::unexpected("Failed to receive data");
      }
      if (received == 0) {
        break;  // Connection closed
      }
      response.append(buffer.data(), static_cast<size_t>(received));
    }

    if (response.empty()) {
      return std::unexpected("Received empty response");
    }

    return response;
  }
};

int main() {
  std::println("=== HTTP Client Demo ===\n");

  HttpClient client;

  // Example 1: Simple GET request
  std::println("1. Simple GET request to http://httpbin.org/get");
  std::println("---");

  auto response1 = client.get("http://httpbin.org/get");
  if (response1) {
    response1->print();
  } else {
    std::println("Error: {}", response1.error());
  }

  std::println("\n");

  // Example 2: GET request with custom headers
  std::println("2. GET request with custom headers");
  std::println("---");

  Headers custom_headers = {{"User-Agent", "CustomHTTPClient/1.0"},
                            {"Accept", "application/json"}};

  auto response2 = client.get("http://httpbin.org/headers", custom_headers);
  if (response2) {
    response2->print();
  } else {
    std::println("Error: {}", response2.error());
  }

  std::println("\n");

  // Example 3: POST request with JSON body
  std::println("3. POST request with JSON body");
  std::println("---");

  Headers json_headers = {{"Content-Type", "application/json"}};
  std::string json_body =
      R"({"name": "John Doe", "email": "john@example.com"})";

  auto response3 =
      client.post("http://httpbin.org/post", json_body, json_headers);
  if (response3) {
    response3->print();
  } else {
    std::println("Error: {}", response3.error());
  }

  std::println("\n");

  // Example 4: Handling different status codes
  std::println("4. Testing different status codes");
  std::println("---");

  auto response404 = client.get("http://httpbin.org/status/404");
  if (response404) {
    std::println("Status: {} {}", response404->status_code,
                 response404->status_message);
    std::println("Is success: {}", response404->is_success());
  }

  std::println("\n");

  // Example 5: Error handling (invalid URL)
  std::println("5. Error handling - invalid URL");
  std::println("---");

  auto response_err = client.get("not-a-valid-url");
  if (!response_err) {
    std::println("Expected error: {}", response_err.error());
  }

  std::println("\n=== Demo Complete ===");
  return 0;
}
