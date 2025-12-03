cmake_minimum_required(VERSION 3.18.0 FATAL_ERROR)
# CMP0000: Call the cmake_minimum_required() command at the beginning of the top-level
# CMakeLists.txt file even before calling the project() command.
# The cmake_minimum_required(VERSION) command implicitly invokes the cmake_policy(VERSION)
# command to specify that the current project code is written for the given range of CMake
# versions.
project(1g4-mixer)


set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

include(GNUInstallDirs)

# Minimum Versions
set(GLIB_MINIMUM_VERSION "2.50.0")
set(QT_MINIMUM_VERSION "6.6.0")

find_package(Qt6 ${QT_MINIMUM_VERSION} COMPONENTS REQUIRED Widgets)

pkg_check_modules(GLIB REQUIRED glib-2.0>=${GLIB_MINIMUM_VERSION})

find_package(PkgConfig REQUIRED)
pkg_check_modules(
    PULSE REQUIRED
    libpulse>=5.0
    libpulse-mainloop-glib>=0.9.16
)

add_subdirectory(src)
