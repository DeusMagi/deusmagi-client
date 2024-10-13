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

if [[ $1 == "package" ]]; then
    echo "Packaging ..."
    
    ARCH=x86_64 appimagetool build/AppDir build/DeusMagi.AppImage
    
    exit 0
fi

if [[ $1 == "debug" ]]; then
    echo "Debugging ..."
    
    cd build
    gdb -ex "set disable-randomization off" ./DeusMagi.AppImage
    cd ..
    
    exit 0
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
