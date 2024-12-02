# CMakeList.txt : Top-level CMake project file,
# Do global configuration and include sub-projects here.
#
cmake_minimum_required (VERSION 3.24)

##########################################################################
#
#   PROJECT
#

# VERSION default "0.86":
if(NOT DEFINED APP_VERSION)

  if(NOT DEFINED APP_VERSION_MAJOR)
    set(APP_VERSION_MAJOR 0)
  endif()
  if(NOT DEFINED APP_VERSION_MINOR)
    set(APP_VERSION_MINOR 86)
  endif()
  if(NOT DEFINED APP_VERSION_PATCH)
    set(APP_VERSION_PATCH 0)
  endif()

  set(APP_VERSION "${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_PATCH}")

endif()

project(
    GAViewer
  VERSION "${APP_VERSION}"
  LANGUAGES C CXX
  DESCRIPTION "Geometric Algebra Viewer"
)

# Apps & libraries versioning:
set(APP_VER "${APP_VERSION_MAJOR}${APP_VERSION_MINOR}")
set(APP_VER_CFG "${APP_VER}$<$<CONFIG:Debug>:d>")

# Auxiliary modules in a separate cmake folder:
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
#list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

##########################################################################
#
#   OPTIONS
#

# There are so many warnings in all the packages in use that it is
# worth eliminating at least some of them (the least dangerous ones):
if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC")
  add_compile_options(-D_CRT_SECURE_NO_WARNINGS)
elseif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "GNU")
  add_compile_options(
    -Wno-deprecated
    # -Wno-deprecated-declarations
    # -Wno-unused-function
    # -Wno-unused-parameter
    # -Wno-unused-variable
  )
endif()

#########
# GNU standard directories for installation
include(GNUInstallDirs)

#########
# Check whether the compiler supports IPO/LTO (interprocedural optimization):
include(CheckIPOSupported OPTIONAL RESULT_VARIABLE check_ipo_module_found)
if(check_ipo_module_found)
  check_ipo_supported(
    RESULT LTO_OPTIMIZATION_SUPPORTED
    OUTPUT LTO_OPTIMIZATION_ERROR
    LANGUAGES C CXX
  )
else()
  set(LTO_OPTIMIZATION_SUPPORTED NO)
  set(LTO_OPTIMIZATION_ERROR "CheckIPOSupported module not found.")
endif()

#########
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

# # GAViewer (multi-purpose program for geometric algebra computations and visualization)
# add_subdirectory("GAViewer-0.86")

# GAViewer (multi-purpose program for geometric algebra computations and visualization)
add_subdirectory("viewer")
