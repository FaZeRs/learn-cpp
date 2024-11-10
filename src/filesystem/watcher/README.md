# File Watcher

A modern C++ file system monitoring utility that watches directories for file changes in real-time.

## Features

- Recursive directory monitoring
- Detection of file/directory creation, modification, and deletion
- Configurable polling interval
- Skip symbolic links
- Permission-denied handling
- Thread-safe implementation using `std::jthread`
- Modern error handling using `std::expected` and `std::error_code`

## Requirements

- C++23 or later
- CMake 3.20 or later
- A compiler supporting:
  - `std::expected`
  - `std::print`/`std::println`
  - Ranges library
  - Filesystem library

## Usage

Here's a basic example of how to use the File Watcher:

```cpp
#include <iostream>

int main() {
    FileWatcher watcher({
        .path_to_watch = ".",
        .callback = [](const std::filesystem::path& path,
                      std::string_view action,
                      std::filesystem::file_type type,
                      std::uintmax_t size) {
            std::println("{} ({} bytes) was {}", 
                        path.string(), size, action);
        },
        .polling_interval = std::chrono::seconds(1)
    });

    if (auto result = watcher.start(); result) {
        auto stop_source = std::move(*result);
        // Keep watching until user presses Enter
        std::cin.get();
        stop_source.request_stop();
    }
    return 0;
}
```

## API Reference

### FileWatcher Class

#### Configuration

The `FileWatcher` is configured using the `Config` struct:

```cpp
struct Config {
    std::filesystem::path path_to_watch;     // Directory to monitor
    WatcherCallback callback;                // Callback for file events
    std::chrono::duration<int> polling_interval = 1s; // Default: 1 second
};
```

#### Callback Function

The callback function receives the following parameters:
- `path`: The path to the affected file/directory
- `action`: The type of change ("created", "modified", or "deleted")
- `type`: The file type (regular file, directory, etc.)
- `size`: The size of the file in bytes

#### Error Handling

The watcher uses custom error codes:
- `PathNotFound`: The specified watch path doesn't exist
- `WatcherStartFailed`: Failed to start the watcher thread
- `ScanFailed`: Failed to scan the directory

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Implementation Details

The File Watcher uses a polling-based approach to detect changes:
1. Recursively scans the watched directory at specified intervals
2. Maintains a hash map of file paths and their metadata
3. Compares current scan results with previous state to detect changes
4. Triggers callbacks for created, modified, or deleted files

## Limitations

- Uses polling instead of native file system events
- May miss rapid successive changes between polling intervals
- Resource usage scales with directory size and polling frequency
