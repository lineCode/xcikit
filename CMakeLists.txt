cmake_minimum_required(VERSION 3.7)
cmake_policy(VERSION 3.7)
project(xcikit C CXX)

option(XCI_WITH_OPENGL "Link with GLFW and build libxci-graphics-opengl." ON)
option(XCI_WITH_MAGNUM "Link with Magnum and build libxci-graphics-magnum." OFF)
option(XCI_WITH_SFML "Link with SFML and build libxci-graphics-sfml." OFF)
option(XCI_WITH_PANDA3D "Link with Panda3D and build libxci-graphics-panda." OFF)
option(XCI_WITH_TINFO "Link with TInfo (from NCurses) and use it for TTY control sequences." OFF)
option(XCI_BENCHMARK "Enable benchmarks. Requires Google benchmark to be installed and in CMake path." OFF)

set(XCI_SHARE_DIR "${CMAKE_CURRENT_LIST_DIR}/share" CACHE PATH "Location of share dir (runtime data)")
option(XCI_EMBED_SHADERS "Embed shader files into binaries to avoid dependency on share dir." OFF)
option(XCI_DEBUG_TRACE "Enable trace log messages." OFF)
option(XCI_DEBUG_MARKUP_DUMP_TOKENS "Text markup parser debugging." OFF)
option(XCI_DEBUG_OPENGL "Log info about OpenGL calls and errors." OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(XciBuildOptions)

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(KEEP_RPATHS)
endif()


# ------------ #
# Sources      #
# ------------ #

set(XCI_TEXT_SOURCES
    src/xci/text/FontLibrary.cpp
    src/xci/text/FontTexture.cpp
    src/xci/text/Font.cpp
    src/xci/text/Text.cpp
    src/xci/text/Layout.cpp
    src/xci/text/Markup.cpp
    src/xci/text/Style.cpp
    src/xci/text/layout/Page.cpp
    src/xci/text/freetype/FtFontLibrary.cpp
    src/xci/text/freetype/FtFontFace.cpp)

set(XCI_WIDGETS_SOURCES
    src/xci/widgets/Button.cpp
    src/xci/widgets/Checkbox.cpp
    src/xci/widgets/FpsDisplay.cpp
    src/xci/widgets/Icon.cpp
    src/xci/widgets/Theme.cpp
    src/xci/widgets/Widget.cpp
    src/xci/widgets/TextInput.cpp
    src/xci/widgets/Form.cpp
    src/xci/widgets/Label.cpp
    src/xci/widgets/TextTerminal.cpp)


# ------------ #
# Dependencies #
# ------------ #

find_package(Threads REQUIRED)

# FreeType
include(FindFreetype)

# RectangleBinPack
add_subdirectory(ext/rbp)
set(RBP_LIBRARY rbp)

# PEGTL
find_package(PEGTL REQUIRED)

# Abseil
find_package(Abseil REQUIRED)

# Catch
find_package(Catch)

# Doxygen
find_package(Doxygen)

# TInfo (ncurses)
if (XCI_WITH_TINFO)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(TINFO REQUIRED tinfo)
endif ()

# Generate config.h
configure_file(config.h.in include/xci/config.h)

# ------------ #
# Libraries    #
# ------------ #

set(XCI_INCLUDE_DIRS
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>)

# Setup RPATH
if (APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()

add_subdirectory(src/xci/core)

add_subdirectory(src/xci/graphics)

# Choose default graphics engine:
if (XCI_WITH_OPENGL)
    add_library(xci-graphics ALIAS xci-graphics-opengl)
elseif (XCI_WITH_SFML)
    add_library(xci-graphics ALIAS xci-graphics-sfml)
elseif (XCI_WITH_PANDA3D)
    add_library(xci-graphics ALIAS xci-graphics-panda3d)
endif()

add_library(xci-text ${XCI_TEXT_SOURCES}
    $<TARGET_OBJECTS:${RBP_LIBRARY}>)
target_include_directories(xci-text
    PUBLIC
        ${XCI_INCLUDE_DIRS}
        ${FREETYPE_INCLUDE_DIRS}
        ${PEGTL_INCLUDE_DIRS}
    PRIVATE
        ext)
target_link_libraries(xci-text
    xci-core
    ${FREETYPE_LIBRARIES})

if (BUILD_FRAMEWORKS)
    set_target_properties(xci-text PROPERTIES
        FRAMEWORK TRUE
        FRAMEWORK_VERSION A
        MACOSX_FRAMEWORK_IDENTIFIER cz.xci.xci-text
        # "current version" in semantic format in Mach-O binary file
        VERSION 1.0.0
        # "compatibility version" in semantic format in Mach-O binary file
        SOVERSION 1.0.0
        PUBLIC_HEADER xci-text.h
        )
endif()

install(TARGETS xci-text EXPORT xcikit DESTINATION lib)

add_library(xci-widgets ${XCI_WIDGETS_SOURCES})
target_link_libraries(xci-widgets
    xci-text)
target_include_directories(xci-widgets
    PRIVATE
        ${CONAN_INCLUDE_DIRS_INCBIN}
)

if (BUILD_FRAMEWORKS)
    set_target_properties(xci-widgets PROPERTIES
        FRAMEWORK TRUE
        FRAMEWORK_VERSION A
        MACOSX_FRAMEWORK_IDENTIFIER cz.xci.xci-widgets-opengl
        # "current version" in semantic format in Mach-O binary file
        VERSION 1.0.0
        # "compatibility version" in semantic format in Mach-O binary file
        SOVERSION 1.0.0
        PUBLIC_HEADER xci-widgets.h
        )
endif()

install(TARGETS xci-widgets EXPORT xcikit DESTINATION lib)


# ------------ #
# Examples     #
# ------------ #

add_subdirectory(examples)


# ------------ #
# Tests        #
# ------------ #

if (Catch_FOUND)
    enable_testing()
    add_subdirectory(tests)
endif ()


# ------------ #
# Benchmarks   #
# ------------ #

if (XCI_BENCHMARK)
    # Google Benchmark
    find_package(benchmark REQUIRED)
    add_subdirectory(benchmarks)
endif ()


# ------------- #
# Documentation #
# ------------- #

# Only with CMake v3.9+ (older versions don't have doxygen_add_docs())
if (DOXYGEN_FOUND AND NOT CMAKE_VERSION VERSION_LESS "3.9")
    set(DOXYGEN_OUTPUT_DIRECTORY doc)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
    set(DOXYGEN_SOURCE_BROWSER YES)
    set(DOXYGEN_JAVADOC_AUTOBRIEF YES)
    set(DOXYGEN_SORT_MEMBER_DOCS NO)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
    doxygen_add_docs(doc
        src/xci
        docs
        README.md
        COMMENT "Generate documentation")
endif ()


# ------------- #
# Installation  #
# ------------- #

install(DIRECTORY src/xci DESTINATION include
    FILES_MATCHING
    REGEX "xci/(graphics|core)/[a-z]+$" EXCLUDE  # internal headers
    PATTERN "*.h")

# config.h
install(DIRECTORY ${CMAKE_BINARY_DIR}/include/xci DESTINATION include)

install(EXPORT xcikit
    NAMESPACE xcikit::
    DESTINATION lib/cmake/xcikit
    FILE "xcikitTargets.cmake")

# Make our CMake modules available to clients
install(FILES cmake/FindCatch.cmake DESTINATION lib/cmake/xcikit)
install(FILES cmake/FindAbseil.cmake DESTINATION lib/cmake/xcikit)

set(XCI_INCLUDE_DIRS include)
include(CMakePackageConfigHelpers)
configure_package_config_file(xcikitConfig.cmake.in xcikitConfig.cmake
    INSTALL_DESTINATION lib/cmake/xcikit
    PATH_VARS XCI_INCLUDE_DIRS)
write_basic_package_version_file(xcikitConfigVersion.cmake
    VERSION 0.1.0
    COMPATIBILITY SameMajorVersion)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/xcikitConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/xcikitConfigVersion.cmake
    DESTINATION lib/cmake/xcikit)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/XciBuildOptions.cmake
    DESTINATION lib/cmake/xcikit)
