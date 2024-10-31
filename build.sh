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
    
    rm -Rf build/AppDir/lib
    
    mkdir -p build/AppDir/lib
    mkdir -p build/AppDir/lib/dri
    
    # copy the graphic drivers
    cp /usr/lib/x86_64-linux-gnu/dri/* build/AppDir/lib/dri
    
    # copy the dependencies
    ldd build/AppDir/deusmagi \
        | grep so \
        | sed -e '/^[^\t]/ d' \
        | sed -e 's/\t//' \
        | sed -e 's/.*=..//' \
        | sed -e 's/ (0.*)//' \
        | while read -r depend; do \
            cp $depend build/AppDir/lib/ 2>/dev/null; \
          done
    
    # copy the libraries used by MESA
    cp /usr/lib/x86_64-linux-gnu/libLLVM-15.so.1 build/AppDir/lib/
    cp /usr/lib/x86_64-linux-gnu/libGL.so.1 build/AppDir/lib/
    cp /usr/lib/x86_64-linux-gnu/libGLU.so.1 build/AppDir/lib/
    cp /usr/lib/x86_64-linux-gnu/libGLX.so.0 build/AppDir/lib/
    cp /usr/lib/x86_64-linux-gnu/libGLX_mesa.so.0 build/AppDir/lib/
    
    # remove unnecessary files
    rm -f build/AppDir/deusmagi.log
    rm -f build/AppDir/server/hfiles
    rm -f build/AppDir/lib/ld-linux-x86-64.so.2
    rm -f build/AppDir/lib/libc.so.6
    
    ARCH=x86_64 appimagetool -n build/AppDir build/DeusMagi.AppImage
    
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
