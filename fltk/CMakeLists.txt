# Fast Light Tool Kit (cross-platform C++ GUI toolkit)
#

message(DEBUG "Entering the FLTK subdirectory ...")

Include(FetchContent)

FetchContent_Declare(
  fltk
  GIT_REPOSITORY https://github.com/fltk/fltk
  GIT_TAG         master    # release-1.4.0-1	# November, 2024
  GIT_SHALLOW     TRUE
)

FetchContent_MakeAvailable(fltk)
# if(TARGET fltk::fltk)
#     message(STATUS "fltk::fltk target found!!!")
# else()
#     message(STATUS "fltk::fltk target NOT found!!!")
# endif()
