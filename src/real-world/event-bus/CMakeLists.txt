add_executable(event-bus main.cpp)
target_compile_features(event-bus PRIVATE cxx_std_23)

# Enable threading support
find_package(Threads REQUIRED)
target_link_libraries(event-bus PRIVATE Threads::Threads)