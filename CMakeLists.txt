# CMakeList.txt : Top-level CMake project file,
# Do global configuration and include sub-projects here.
#
cmake_minimum_required (VERSION 3.25)

##########################################################################
#
#   PROJECT
#

# VERSION default "0.86.0":
set(APP_VERSION_MAJOR 0)
set(APP_VERSION_MINOR 86)
set(APP_VERSION_PATCH 0)
set(APP_VERSION "${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_PATCH}")

project(
    GAViewer
  VERSION "${APP_VERSION}"
  LANGUAGES C CXX
  DESCRIPTION "Geometric Algebra Viewer"
)

# C++ language standard to compile for
set(CMAKE_CXX_STANDARD 14)

# To avoid portability issues, language extensions (enabled by default)
# are disabled before any target is created. This allows to stick to the
# language standard such as "-std=c++17" instead of "-std=gnu++17" (which
# is c++17 with GNU extensions):
set(CMAKE_CXX_EXTENSIONS OFF)

##########################################################################
#
#   GLOBAL CONFIGURATION
#

####
# CMake helper modules folder:
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

####
# GNU standard directories for installation
include(GNUInstallDirs)

####
# Check Git availability and get GIT_EXECUTABLE path
# (Git is required not only for downloading but also for patching repositories):
find_package(Git REQUIRED)

####
# USE_ASAN option:
option(USE_ASAN "Use Address Sanitizer: OFF/ON" OFF)
message(DEBUG "Use Address Sanitizer: ${USE_ASAN}")

# Address Sanitizer requires a static runtime library, but on Windows it defaults to MSVC's dynamic library.
if(USE_ASAN)
  # Select the static MSVC runtime (for compilers targeting the MSVC ABI):
  set(
    CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    CACHE STRING "MSVC Runtime Library for compilers targeting the MSVC ABI"
    FORCE
  )
  #
  # NOTE: I have not been able to get Address Sanitizer to work on Windows
  # (regardless of the compiler used)!!!
  #
endif()

##########################################################################
#
#   TESTS
#
#   Use the following command (in the project's root directory) to
#   run all tests defined in the project:
#
#       ctest --test-dir ./build/ [-V[V]] 
#

#enable_testing()

##########################################################################
#
#   SOURCE PATHS
#

#cmake_path(SET ROOT_DIR "${PROJECT_SOURCE_DIR}")

##########################################################################
#
#   TARGETS to BUILD
#

# Fast Light Tool Kit (cross-platform C++ GUI toolkit, FLTK).
# Integral to the FLTK distribution are the following packages: 
# - data compression library (zlib),
# - PNG reference library (libpng).
add_subdirectory ("fltk")

# ANTLR v2 stable version is 2.7.7 (released November 1, 2006)
add_subdirectory ("antlr2")

# GAViewer (multi-purpose program for geometric algebra computations and visualization)
# add_subdirectory("viewer")
add_subdirectory("viewer2")

# Interactive Book Figures for GAViewer
add_subdirectory("figures")

# FLTK Editor
add_subdirectory("editor")
