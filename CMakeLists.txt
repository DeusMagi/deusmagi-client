# The Atrinik CMakeLists file.

project(atrinik C)
cmake_minimum_required(VERSION 3.2)

# Include build configuration.
include(build.config)

if (EXISTS build.user)
    include(build.user)
endif ()

set(EXECUTABLE atrinik)
set(EXECUTABLE_UPDATER atrinik2)
set(PACKAGE_NAME "Atrinik Client")

# Add our includes.
include_directories(common)
include_directories(src/include)
include_directories(src/gui/toolkit/include)
include_directories(src)

# Add some useful compile flags.
add_definitions(-Wall -Wextra -Wno-unused-parameter -Wno-deprecated-declarations -Wno-format-truncation -Wno-format-overflow -Wno-implicit-fallthrough -Wno-int-conversion -Wno-cast-function-type -Wno-discarded-qualifiers -Wno-implicit-function-declaration -Wno-missing-field-initializers -Wno-unused-function -D_GNU_SOURCE -D__USE_MINGW_ANSI_STDIO=0 -std=gnu99 -ggdb -O0)
add_definitions(${CUSTOM_WARNINGS})

if (NOT MINGW)
    if (ENABLE_STACK_PROTECTOR)
        add_definitions(-fstack-protector-all -Wstack-protector)
    else ()
        add_definitions(-fno-stack-protector)
    endif ()
endif ()

if (ENABLE_EXTRA_WARNINGS)
    # Non-macro identifiers inside #if outside of defined()
    add_definitions(-Wundef)
    # sizeof() on function types or void
    add_definitions(-Wpointer-arith)
    # Casting function types to non-matching types
    add_definitions(-Wbad-function-cast)
    # extern declaration within a function
    add_definitions(-Wnested-externs)
    # Dead code (removed in newer versions of GCC, might still be in clang)
    add_definitions(-Wunreachable-code)
    # Additional format checks for printf/scanf/etc not enabled by -Wall
    add_definitions(-Wformat=2 -Wno-format-nonliteral)
    # Declarations like int i = i
    add_definitions(-Winit-self)
    # Local variables or types that shadow another variable, parameter, type,
    # built-in function, etc.
    add_definitions(-Wshadow)
    # Global function defined without a previous declaration.
    add_definitions(-Wmissing-declarations)
    # Variable that may be clobbered by a setjmp call
    add_definitions(-Wuninitialized)
    # Testing floating point values for equality
    add_definitions(-Wfloat-equal)

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        # Suspicious uses of logical operators in expressions
        add_definitions(-Wlogical-op)
        # Pointer casting with increased alignment (char * -> int *)
        add_definitions(-Wcast-align)
    endif ()
endif ()

if (ENABLE_AGGRESSIVE_WARNINGS)
    # Casting const char * to char *
    add_definitions(-Wcast-qual)
    # Assigning signed int to unsigned int
    add_definitions(-Wsign-conversion)
    # Loop indices that could potentially overflow
    add_definitions(-Wunsafe-loop-optimizations)
    # Redundant (multiple same) declarations
    add_definitions(-Wredundant-decls)
endif ()

# Turn warnings into errors?
if (ENABLE_WARNING_ERRORS)
    add_definitions(-Werror)
endif ()

if (MINGW)
    add_definitions(-DCURL_STATICLIB -DPTW32_STATIC_LIB -DBGDWIN32 -DNONDLL)
    add_definitions(-DNOCRYPT)
    set(CURL_STATICLIB true)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc")
    set(PLUGIN_SUFFIX ".dll")
    set(CMAKE_PREFIX_PATH "C:\\MinGW\\msys\\1.0;C:\\MinGW")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so;.dll.a")
else ()
    # Compiling on GNU/Linux.
    set(LINUX true)
    set(PLUGIN_SUFFIX ".so")
endif ()

if (MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
endif ()

# SDL is required to run the client.
find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIR})
set(HAVE_SDL true)

# SDL_image is also required, otherwise no bitmaps or map images could be
# displayed.
find_package(SDL_image REQUIRED)
include_directories(${SDLIMAGE_INCLUDE_DIR})
set(HAVE_SDL_IMAGE true)

# SDL_ttf is used for all text drawing, so it's required.
find_package(SDL_ttf REQUIRED)
include_directories(${SDLTTF_INCLUDE_DIR})
set(HAVE_SDL_TTF true)

# LibXML2 is required.
find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

# SDL_mixer is optional, as sound is not a must-have.
find_package(SDL_mixer)

if (SDLMIXER_FOUND)
    include_directories(${SDLMIXER_INCLUDE_DIR})
    set(HAVE_SDL_MIXER true)
else ()
    message(STATUS "Warning: SDL_mixer not found - sound support will be disabled")
endif ()

# Add sources.
include(src/cmake.txt)

if (MINGW)
    ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/atrinikrc.o
            COMMAND ${CMAKE_RC_COMPILER} -I${CMAKE_CURRENT_SOURCE_DIR}
            -i${CMAKE_CURRENT_SOURCE_DIR}/atrinik.rc
            -o ${CMAKE_CURRENT_BINARY_DIR}/atrinikrc.o
            VERBATIM)
    SET(SOURCES ${SOURCES} "${CMAKE_CURRENT_BINARY_DIR}/atrinikrc.o")
    add_executable(${EXECUTABLE_UPDATER}
        src/updater/main.c "${CMAKE_CURRENT_BINARY_DIR}/atrinikrc.o")
endif ()

# Compile the sources into an executable.
add_executable(${EXECUTABLE} WIN32 ${SOURCES})
target_link_libraries(${EXECUTABLE} atrinik-toolkit)
target_link_libraries(${EXECUTABLE} -Xlinker --allow-multiple-definition)

# Link all the available libraries.
target_link_libraries(${EXECUTABLE} ${SDL_LIBRARY})
target_link_libraries(${EXECUTABLE} ${SDLIMAGE_LIBRARY})
target_link_libraries(${EXECUTABLE} ${SDLTTF_LIBRARY})
target_link_libraries(${EXECUTABLE} ${LIBXML2_LIBRARIES})

# Link SDL_mixer if available.
if (HAVE_SDL_MIXER)
    target_link_libraries(${EXECUTABLE} ${SDLMIXER_LIBRARY})
endif ()

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

# Installer.
if (WIN32 AND NOT UNIX)
    set(INSTALL_SUBDIR_BIN ".")
    set(INSTALL_SUBDIR_SHARE ".")
    set(INSTALL_SUBDIR_DOC ".")

    file(GLOB DLLS "${CMAKE_CURRENT_SOURCE_DIR}/*.dll")
    set(EXES gunzip.exe tar.exe atrinik.bat ca-bundle.crt timidity.cfg)
    install(FILES ${DLLS} DESTINATION ${INSTALL_SUBDIR_BIN})
    install(TARGETS atrinik2 DESTINATION ${INSTALL_SUBDIR_BIN})
    install(FILES ${EXES} DESTINATION ${INSTALL_SUBDIR_BIN})
    install(DIRECTORY timidity DESTINATION ${INSTALL_SUBDIR_BIN})
else ()
    set(INSTALL_SUBDIR_BIN "games")
    set(INSTALL_SUBDIR_SHARE "share/games/atrinik")
    set(INSTALL_SUBDIR_DOC "share/doc/atrinik")

    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/atrinik.desktop DESTINATION "share/applications")
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/textures/icon.png DESTINATION "share/pixmaps" RENAME "atrinik.png")
endif ()

install(TARGETS atrinik DESTINATION ${INSTALL_SUBDIR_BIN})
install(FILES
        # ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md
        ${CMAKE_CURRENT_SOURCE_DIR}/client.cfg
        DESTINATION ${INSTALL_SUBDIR_DOC})
install(DIRECTORY textures cache data fonts gfx_user schemas settings sound srv_files DESTINATION ${INSTALL_SUBDIR_SHARE} PATTERN ".git" EXCLUDE)

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Fantasy multiplayer online role-playing game")
set(CPACK_PACKAGE_VENDOR "Atrinik Development Team")
set(CPACK_PACKAGE_CONTACT "Atrinik Development Team <admin@atrinik.org>")
# set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md")
set(CPACK_PACKAGE_VERSION_MAJOR "4")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Atrinik")

if (MINGW)
    set(CPACK_PACKAGE_EXECUTABLES "atrinik2" "Atrinik")
    set(CPACK_CREATE_DESKTOP_LINKS "atrinik2")
endif ()

set(CPACK_DEBIAN_PACKAGE_NAME "atrinik")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.5), libgcc1 (>= 1:4.1), libsdl-image1.2 (>= 1.2.5), libsdl1.2debian (>= 1.2.10-1), libcurl3 (>= 7.16), libsdl-mixer1.2 (>= 1), libsdl-ttf2.0-0 (>= 1), timidity (>= 1), zlib1g (>= 1), libx11-6, libxmu6")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Fantasy multiplayer online role-playing game with 2.5D graphics\n .\n Homepage: http://www.atrinik.org/")
set(CPACK_DEBIAN_PACKAGE_SECTION "games")

set(CPACK_RPM_PACKAGE_NAME "atrinik")
set(CPACK_RPM_PACKAGE_DESCRIPTION "Fantasy multiplayer online role-playing game with 2.5D graphics\n .\n Homepage: http://www.atrinik.org/")
set(CPACK_RPM_PACKAGE_LICENSE "GNU General Public License (GPL)")
set(CPACK_RPM_PACKAGE_GROUP "Games")

set(CPACK_NSIS_INSTALLED_ICON_NAME "atrinik")
set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/textures/icon.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/textures/icon.ico")
set(CPACK_NSIS_DISPLAY_NAME "Atrinik")
set(CPACK_NSIS_MODIFY_PATH "ON")
set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")

if (${PACKAGE_TYPE} STREQUAL "deb")
    set(CPACK_BINARY_DEB "ON")
elseif (${PACKAGE_TYPE} STREQUAL "nsis")
    set(CPACK_BINARY_NSIS "ON")
elseif (${PACKAGE_TYPE} STREQUAL "rpm")
    set(CPACK_BINARY_RPM "ON")
endif (${PACKAGE_TYPE} STREQUAL "deb")

set(CPACK_BINARY_STGZ "OFF")
set(CPACK_BINARY_TGZ "OFF")
set(CPACK_BINARY_TZ "OFF")
include(CPack)

# Configure the .h file with the configuration options (size of long,
# package version, etc).
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/include/cmake.h.def ${CMAKE_CURRENT_SOURCE_DIR}/src/include/cmake.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/include/version.h.def ${CMAKE_CURRENT_SOURCE_DIR}/src/include/version.h)

add_subdirectory(common/toolkit)