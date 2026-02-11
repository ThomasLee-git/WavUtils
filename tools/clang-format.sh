#!/bin/bash

clang_format_path="/dev/shm/LLVM-21.1.8-Linux-X64/bin/clang-format"
declare -a dir_list=("src" "examples")
for d in "${dir_list[@]}"; do
    find "${d}" -type f -name "*.cpp" -o -name "*.hpp" | xargs -I {} ${clang_format_path} -i "{}"
done