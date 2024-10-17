set(SOURCE_FILES main.cpp)

add_executable(parallel ${SOURCE_FILES})

find_package(TBB REQUIRED COMPONENTS tbb)
target_link_libraries(parallel PRIVATE project_options project_warnings TBB::tbb)
