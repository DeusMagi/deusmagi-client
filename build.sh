#!/bin/bash

if [[ $1 == "clean" ]]; then
    echo "Cleaning ..."
    
    rm -Rf build/CMakeCache.txt
    rm -Rf build/CMakeFiles
    rm -Rf build/Makefile
    rm -Rf build/common/toolkit/CMakeCache.txt
    rm -Rf build/common/toolkit/CMakeFiles
    rm -Rf build/common/toolkit/Makefile
fi

cd build

cmake \
    -DPACKAGE_VERSION="0.1.0" \
    -DPACKAGE_VERSION_MAJOR=0 \
    -DPACKAGE_VERSION_MINOR=1 \
    -DPACKAGE_VERSION_PATCH=0 \
    -DCMAKE_BUILD_TYPE="Debug" \
    .

make

cd ..
