#!/bin/bash

find . -type f -name 'CMakeLists.txt' | xargs -I {} cmake-format -i "{}"