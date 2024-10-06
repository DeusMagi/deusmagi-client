project(deusmagi C)
cmake_minimum_required(VERSION 3.12)

set(EXECUTABLE deusmagi)
set(PACKAGE_NAME "Deus Magi Client")

include_directories(common)
include_directories(../src)
include_directories(../src/include)
include_directories(../src/gui/toolkit/include)

add_definitions(-Wall -Wextra -Wno-unused-parameter -Wno-deprecated-declarations -Wno-format-truncation -Wno-format-overflow -Wno-implicit-fallthrough -Wno-int-conversion -Wno-cast-function-type -Wno-discarded-qualifiers -Wno-implicit-function-declaration -Wno-missing-field-initializers -Wno-unused-function -D_GNU_SOURCE -D__USE_MINGW_ANSI_STDIO=0 -std=gnu99 -ggdb -O0)

set(LINUX true)
set(PLUGIN_SUFFIX ".so")

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

configure_file(cmake.h.def ../src/include/cmake.h)
configure_file(version.h.def ../src/include/version.h)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ../src/*)
add_executable(${EXECUTABLE} ${SOURCES} ${SOURCES_TOOLKIT})

target_link_libraries(${EXECUTABLE} deusmagi-toolkit)
target_link_libraries(${EXECUTABLE} -Xlinker --allow-multiple-definition)
target_link_libraries(${EXECUTABLE} ${SDL2_LIBRARIES})
target_link_libraries(${EXECUTABLE} ${SDL2_IMAGE_LIBRARY})
target_link_libraries(${EXECUTABLE} ${SDL2_MIXER_LIBRARY})
target_link_libraries(${EXECUTABLE} ${SDL2_TTF_LIBRARY})
target_link_libraries(${EXECUTABLE} ${LIBXML2_LIBRARIES})

add_subdirectory(common/toolkit)