#!/bin/bash

path_to_install="/dev/shm/wavutils_built"
rm -r "${path_to_install}"
rm -r _build
cmake -B _build -DBUILD_SHARED_LIBS=ON -DCMAKE_PREFIX_PATH="${path_to_install}" || exit -1
cmake --build _build -j8 || exit -1
cmake --install _build || exit -1