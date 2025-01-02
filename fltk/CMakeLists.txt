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
  GIT_TAG        b080424a2358ae805702f39646a424e5a71eb883 # FLTK 1.4.1 released on Dec. 12, 2024
  GIT_SHALLOW    TRUE
)

##########################################################################
#
#	  Patching
#
#

include(PatchFiles)
set(files_to_patch "fluid/CMakeLists.txt")
patch_files("${CMAKE_CURRENT_SOURCE_DIR}" "${files_to_patch}")

##########################################################################
#
#	  Build options
#
#

# FLTK_OPTION_STD option allows FLTK to use some specific features of
# modern C++ like std::string in the public API of FLTK 1.4.x. Users turning
# this option ON can benefit from some new functions and methods that return
# std::string or use std::string as input parameters.
set(FLTK_OPTION_STD         ON)   # default OFF

# Use STATIC libraries regardless of whether they are built for
# Windows (static is the default) or another OS (dynamic may be available):
set(FLTK_USE_SYSTEM_LIBJPEG OFF)  # default ON (macOS and Windows: OFF)
set(FLTK_USE_SYSTEM_LIBPNG  OFF)  # default ON (macOS and Windows: OFF)
set(FLTK_USE_SYSTEM_ZLIB    OFF)  # default ON (macOS and Windows: OFF)

# # When the X11 backend is selected on BSD systems, compilation of
# # the FLUID application may fail (unlike other FLTK components).
# # This is because the default (system) include paths do not contain
# # the X11 directories required by the application, and its CMake script
# # does not try to find them. Here is a workaround for this problem
# # (a better solution would be to implement this workaround directly
# # in the FLUID CMake script - or, as a last resort, to abandon/disable
# # the FLUID application altogether)
# if(BSD)
#   find_package(X11)
#   if (X11_FOUND)
#     set(FLTK_BUILD_FLUID ON)
#     # The following command does what we need (although
#     # additional include dirs may interfere with compilation
#     # of other FLTK components)
#     include_directories(AFTER SYSTEM "${X11_INCLUDE_DIR}")
#   else()
#     set(FLTK_BUILD_FLUID OFF)
#   endif()
# endif()

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
# In addition to the library targets FLTK defines
# the "imported target" 'fltk::fluid'.
#
# (see README.CMake.txt file in the fltk source folder).
