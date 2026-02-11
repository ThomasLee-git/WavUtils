cmake_minimum_required(VERSION 3.15.0)

project(
    WavUtils
    VERSION 0.0.1
    LANGUAGES CXX
    DESCRIPTION "Utilities for WAV audio format"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include(GNUInstallDirs)

# Build options
option(
    WAV_UTILS_BUILD_EXAMPLES
    "Build example executable"
    ON
)

# Build library (static, shared, or both via src/CMakeLists.txt)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Optional example executable
if(WAV_UTILS_BUILD_EXAMPLES)
    add_executable(
        test_wav_utils ${CMAKE_CURRENT_SOURCE_DIR}/examples/test_wav_utils.cpp
    )
    target_include_directories(
        test_wav_utils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    target_link_libraries(test_wav_utils PRIVATE wav_utils)
endif()

# ---- Install (for use by other projects) ----

# Install the export set (targets are defined in src/)
install(
    EXPORT WavUtilsTargets
    FILE WavUtilsTargets.cmake
    NAMESPACE WavUtils::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/WavUtils
)

# Config and version for find_package(WavUtils)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/WavUtilsConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/WavUtilsConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/WavUtils
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/WavUtilsConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/WavUtilsConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/WavUtilsConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/WavUtils
)
