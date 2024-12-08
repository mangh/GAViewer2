# Fast Light Tool Kit (cross-platform C++ GUI toolkit)
#

message(DEBUG "Entering the FLTK subdirectory ...")

##########################################################################
#
#   FLTK directory compile options
#
#   When compiling FLTK source code (which is years old), 
#   many deprecation warnings appear. It's worth turning them off to get
#   a clearer picture of the compilation and spot more serious issues (if any).
#

if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC")

  add_compile_options(
    -D_CRT_SECURE_NO_WARNINGS
    -W3     # production quality warnings
  )

elseif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "GNU")

  add_compile_options(
    -Wall
    -Wno-deprecated
  )

endif()

##########################################################################
#
#   Downloading source files
#
#

Include(FetchContent)

FetchContent_Declare(
  fltk
  GIT_REPOSITORY https://github.com/fltk/fltk
  GIT_TAG        d547e1956778ed584248179d9141843b86101a0a # release-1.4.0-1	# November 18, 2024
  GIT_SHALLOW    TRUE
)

# Use STATIC libraries regardless of whether they are built for
# Windows (static is the default) or another OS (dynamic may be available).
set(FLTK_USE_SYSTEM_LIBJPEG OFF)  # default ON (macOS and Windows: OFF)
set(FLTK_USE_SYSTEM_LIBPNG  OFF)  # default ON (macOS and Windows: OFF)
set(FLTK_USE_SYSTEM_ZLIB    OFF)  # default ON (macOS and Windows: OFF)

# Disable the Wayland backend (default ON)
set(FLTK_BACKEND_WAYLAND OFF)
set(FLTK_OPTION_STD ON) # default OFF
# This option allows FLTK to use some specific features of modern C++
# like std::string in the public API of FLTK 1.4.x. Users turning this
# option ON can benefit from some new functions and methods that return
# std::string or use std::string as input parameters.
# Note: This option will be

FetchContent_MakeAvailable(fltk)

# The previous command provides the following STATIC libraries (targets to IMPORT):
#
#    fltk::fltk  
#    fltk::forms 
#    fltk::gl    
#    fltk::images
#    fltk::jpeg  
#    fltk::png   
#    fltk::z     
#
# (see README.CMake.txt file in the fltk source folder).
