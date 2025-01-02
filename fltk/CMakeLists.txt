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
#	  Patching FLTK 1.4.1 (b080424a2358ae805702f39646a424e5a71eb883)
#
#

# On BSD systems, compilation of the FLUID application (unlike other
# FLTK components) fails for the X11 backend. This happens because
# the X11 directories required by the application are not in the include
# paths, and the FLUID/CMake script does not attempt to fix this.
if(BSD)
  include(PatchFiles)
  set(files_to_patch "fluid/CMakeLists.txt")
  patch_files("${CMAKE_CURRENT_SOURCE_DIR}" "${files_to_patch}")
endif()

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
