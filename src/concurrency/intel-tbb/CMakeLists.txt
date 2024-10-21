set(SOURCE_FILES main.cpp)

add_executable(intel-tbb ${SOURCE_FILES})

find_package(TBB REQUIRED COMPONENTS tbb)
target_link_libraries(intel-tbb PRIVATE project_options project_warnings TBB::tbb)
