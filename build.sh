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
    
    rm -f build/AppDir/deusmagi.log
    ls -Q build/AppDir/server | grep -v .gitignore | xargs rm -f
    
    ARCH=x86_64 appimagetool -n build/AppDir build/DeusMagi.AppImage
    
    exit 0
fi

if [[ $1 == "debug" ]]; then
    echo "Debugging ..."
    
    cd build
    ./DeusMagi.AppImage debug
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
