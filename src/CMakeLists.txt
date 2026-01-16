cmake_minimum_required(VERSION 3.15.0)
project(
    wav_utils
    VERSION 0.0.1
    LANGUAGES CXX
)

set(SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/data_structure.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wav_header.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/file_reader.cpp
)

# add static library
add_library(wav_utils STATIC ${SRC})
