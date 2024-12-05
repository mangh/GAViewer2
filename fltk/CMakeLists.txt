# Fast Light Tool Kit (cross-platform C++ GUI toolkit)
#

message(DEBUG "Entering the FLTK subdirectory ...")

##########################################################################
#
#   Directory compile options
#
#   When compiling FLTK source code (which is years old), 
#   many deprecation warnings appear. It's worth turning them off to get
#   a clearer picture of the compilation and spot more serious issues (if any).
#

if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC")

  add_compile_options(-D_CRT_SECURE_NO_WARNINGS)

elseif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "GNU")

  add_compile_options(-Wno-deprecated)

endif()

##########################################################################
#
#	  Downloading source files
#
#

Include(FetchContent)

FetchContent_Declare(
  fltk
  GIT_REPOSITORY https://github.com/fltk/fltk
  GIT_TAG        d547e1956778ed584248179d9141843b86101a0a # release-1.4.0-1	# November 18, 2024
  GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(fltk)

# The previous command provides the following targets:
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
