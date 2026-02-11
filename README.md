# WavUtils

Utilities for WAV audio format (C++17).

## Build options

- **Static (default):** build a static library only.
- **Shared:** build a shared library:  
  `cmake -B build -DBUILD_SHARED_LIBS=ON`
- **Both:** build static and shared in one go:  
  `cmake -B build -DWAV_UTILS_BUILD_BOTH=ON`
- **Examples:** build the example executable (default ON):  
  `cmake -B build -DWAV_UTILS_BUILD_EXAMPLES=OFF`

## Build and install

```bash
cmake -B build -DCMAKE_INSTALL_PREFIX=/path/to/install
cmake --build build
cmake --install build
```

Install layout (using default `GNUInstallDirs`):

- Headers: `<prefix>/include/`
- Libraries: `<prefix>/lib/` (or `lib64/` on some systems)
- CMake config: `<prefix>/lib/cmake/WavUtils/`

## Use from another project

After installing, use from your project with:

```cmake
find_package(WavUtils 0.0.1 REQUIRED)
target_link_libraries(your_target PRIVATE WavUtils::wav_utils)
```

If you built both static and shared, `WavUtils::wav_utils` refers to the static library. To use the shared library explicitly:

```cmake
target_link_libraries(your_target PRIVATE WavUtils::wav_utils_shared)
```

Configure your project with the install prefix so CMake can find WavUtils, e.g.:

```bash
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/install
```
