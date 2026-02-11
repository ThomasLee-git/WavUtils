cmake_minimum_required(VERSION 3.15.0)
project(
    wav_utils
    VERSION 0.0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Option: build both static and shared (default: only one, controlled by BUILD_SHARED_LIBS)
option(
    WAV_UTILS_BUILD_BOTH
    "Build both static and shared libraries"
    OFF
)

set(SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/data_structure.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wav_header.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/file_reader.cpp
)

set(PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/data_structure.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/file_reader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/file_writer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wav_header.hpp
)

if(WAV_UTILS_BUILD_BOTH)
    # Build both static and shared
    add_library(wav_utils_static STATIC ${SRC})
    add_library(wav_utils_shared SHARED ${SRC})
    target_include_directories(
        wav_utils_static
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
               $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_include_directories(
        wav_utils_shared
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
               $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    set_target_properties(
        wav_utils_shared
        PROPERTIES OUTPUT_NAME wav_utils
                   SOVERSION ${PROJECT_VERSION_MAJOR}
                   VERSION ${PROJECT_VERSION}
    )
    # Alias for backward compatibility: wav_utils -> static when building both
    add_library(
        wav_utils
        ALIAS
        wav_utils_static
    )
    set(WAV_UTILS_INSTALL_TARGETS wav_utils_static wav_utils_shared)
else()
    # Single library: static or shared based on BUILD_SHARED_LIBS
    add_library(wav_utils ${SRC})
    target_include_directories(
        wav_utils PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    if(BUILD_SHARED_LIBS)
        set_target_properties(
            wav_utils PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR}
                                 VERSION ${PROJECT_VERSION}
        )
    endif()
    set(WAV_UTILS_INSTALL_TARGETS wav_utils)
endif()

# Install library target(s)
include(GNUInstallDirs)
install(
    TARGETS ${WAV_UTILS_INSTALL_TARGETS}
    EXPORT WavUtilsTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install public headers
install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Export targets for config package (generated in root CMakeLists.txt)
set(WAV_UTILS_EXPORT_NAME
    WavUtilsTargets
    PARENT_SCOPE
)
