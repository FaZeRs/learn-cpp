# Thread Pool Implementation in C++

A modern C++20 thread pool implementation that manages a collection of worker threads to execute tasks asynchronously. This implementation uses standard library components and provides a simple interface for enqueueing tasks and retrieving their results.

## Features

- Modern C++20 implementation
- Uses `std::jthread` for automatic thread management
- Thread-safe task queue
- Support for tasks with arbitrary arguments and return types
- Automatic thread count detection based on hardware
- Clean shutdown mechanism
- Exception-safe design

## Implementation Details

The thread pool implementation (see `main.cpp`) consists of several key components:

1. **Worker Threads**: Created using `std::jthread` for automatic joining on destruction
2. **Task Queue**: A thread-safe queue of tasks implemented using `std::queue`
3. **Synchronization**: Uses `std::mutex` and `std::condition_variable_any` for thread synchronization
4. **Task Packaging**: Utilizes `std::packaged_task` and `std::future` for handling task results

### Key Features Explained

#### Automatic Thread Management
The pool automatically determines the optimal number of threads using `std::thread::hardware_concurrency()` if not specified.

#### Task Enqueueing
Tasks are enqueued using the `enqueue` method, which:
- Accepts callable objects with any number of arguments
- Returns a `std::future` for retrieving the result
- Uses perfect forwarding for efficient argument passing

#### Thread Safety
The implementation ensures thread safety through:
- Mutex-protected task queue access
- Condition variable for worker thread synchronization
- RAII-based lock management

#### Graceful Shutdown
The destructor ensures a clean shutdown by:
- Requesting all worker threads to stop
- Notifying all waiting threads
- Automatically joining threads (via `std::jthread`)
