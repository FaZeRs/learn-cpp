set(SOURCE_FILES main.cpp)

add_executable(omp ${SOURCE_FILES})

target_link_libraries(omp PRIVATE project_options project_warnings)

find_package(OpenMP)

if(OpenMP_CXX_FOUND)
  target_link_libraries(omp PUBLIC OpenMP::OpenMP_CXX)
endif()
