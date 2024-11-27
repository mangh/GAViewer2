# PNG Reference Library
#

message(STATUS "Build libpng package ...")

Include(FetchContent)

FetchContent_Declare(
  libpng
  GIT_REPOSITORY https://github.com/pnggroup/libpng
  GIT_TAG        v1.6.44   # September 12, 2024
)

FetchContent_MakeAvailable(libpng)
