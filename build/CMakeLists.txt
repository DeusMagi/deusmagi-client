project(deusmagi C)
cmake_minimum_required(VERSION 3.2)

set(EXECUTABLE deusmagi)
set(PACKAGE_NAME "Deus Magi Client")

include_directories(common)
include_directories(../src)
include_directories(../src/include)
include_directories(../src/gui/toolkit/include)

add_definitions(-Wall -Wextra -Wno-unused-parameter -Wno-deprecated-declarations -Wno-format-truncation -Wno-format-overflow -Wno-implicit-fallthrough -Wno-int-conversion -Wno-cast-function-type -Wno-discarded-qualifiers -Wno-implicit-function-declaration -Wno-missing-field-initializers -Wno-unused-function -D_GNU_SOURCE -D__USE_MINGW_ANSI_STDIO=0 -std=gnu99 -ggdb -O0)

if (MINGW)
    add_definitions(-DCURL_STATICLIB -DPTW32_STATIC_LIB -DBGDWIN32 -DNONDLL)
    add_definitions(-DNOCRYPT)
    set(CURL_STATICLIB true)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc")
    set(PLUGIN_SUFFIX ".dll")
    set(CMAKE_PREFIX_PATH "C:\\MinGW\\msys\\1.0;C:\\MinGW")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so;.dll.a")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
else ()
    set(LINUX true)
    set(PLUGIN_SUFFIX ".so")
endif ()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
set(HAVE_SDL true)

find_package(SDL2_image REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIR})
set(HAVE_SDL_IMAGE true)

find_package(SDL2_mixer REQUIRED)
include_directories(${SDL2_MIXER_INCLUDE_DIR})
set(HAVE_SDL_MIXER true)

find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_TTF_INCLUDE_DIR})
set(HAVE_SDL_TTF true)

find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

include(../src/cmake.txt)

add_executable(${EXECUTABLE} WIN32 ${SOURCES})

target_link_libraries(${EXECUTABLE} deusmagi-toolkit)
target_link_libraries(${EXECUTABLE} -Xlinker --allow-multiple-definition)
target_link_libraries(${EXECUTABLE} ${SDL2_LIBRARIES})
target_link_libraries(${EXECUTABLE} ${SDL2_IMAGE_LIBRARY})
target_link_libraries(${EXECUTABLE} ${SDL2_MIXER_LIBRARY})
target_link_libraries(${EXECUTABLE} ${SDL2_TTF_LIBRARY})
target_link_libraries(${EXECUTABLE} ${LIBXML2_LIBRARIES})

# Get all the include directories used to compile the client.
get_directory_property(INCLUDES INCLUDE_DIRECTORIES)

# Go through the include directories and construct a string.
foreach (var ${INCLUDES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" var ${var})
    set(INCLUDES_STRING "${INCLUDES_STRING} -I${var}")
endforeach ()

# Go through the source files and construct a string.
foreach (var ${SOURCES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" var ${var})
    set(SOURCES_STRING "${SOURCES_STRING};${var}")
endforeach ()

configure_file(../src/include/cmake.h.def ../src/include/cmake.h)
configure_file(../src/include/version.h.def ../src/include/version.h)

add_subdirectory(common/toolkit)