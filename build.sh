#!/bin/bash

if [[ $1 == "clean" ]]; then
    echo "Cleaning ..."
    
    rm -Rf build/CMakeCache.txt
    rm -Rf build/CMakeFiles
    rm -Rf build/common/toolkit/CMakeCache.txt
    rm -Rf build/common/toolkit/CMakeFiles
fi

cd build

cmake \
    -DPACKAGE_VERSION="4.0" \
    -DPACKAGE_VERSION_MAJOR=4 \
    -DPACKAGE_VERSION_MINOR=0 \
    -DPACKAGE_VERSION_PATCH=0 \
    -DCMAKE_BUILD_TYPE="Debug" \
    .

make

cd ..
