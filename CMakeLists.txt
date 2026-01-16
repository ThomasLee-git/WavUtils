cmake_minimum_required(VERSION 3.15.0)

project(
    test_wav_utils
    VERSION 0.0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# build modules
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

# build executable
add_executable(
    test_wav_utils ${CMAKE_CURRENT_SOURCE_DIR}/examples/test_wav_utils.cpp
)
target_include_directories(
    test_wav_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(test_wav_utils PUBLIC wav_utils)
