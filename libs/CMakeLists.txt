# GAViewer required libraries
#

Include(FetchContent)

# Fast Light Tool Kit (cross-platform C++ GUI toolkit)
message(STATUS "*** DECLARE fltk")
FetchContent_Declare(
  fltk
  GIT_REPOSITORY https://github.com/fltk/fltk
  GIT_TAG        release-1.4.0-1	# November, 2024
)

# PNG Reference Library
message(STATUS "*** DECLARE libpng")
FetchContent_Declare(
  libpng
  GIT_REPOSITORY https://github.com/pnggroup/libpng
  GIT_TAG        v1.6.44   # September 12, 2024
)

# zlib data compression library
message(STATUS "*** DECLARE zlib")
FetchContent_Declare(
  zlib
  GIT_REPOSITORY https://github.com/madler/zlib
  GIT_TAG        v1.3.1   # January 22, 2024
)

message(STATUS "*** MAKEAVAILABLE zlib")
FetchContent_MakeAvailable(
  zlib
)

FetchContent_GetProperties(zlib)

set(ZLIB_INCLUDE_DIR "${zlib_SOURCE_DIR}")
set(ZLIB_LIBRARY "${zlib_BINARY_DIR}")

message(STATUS "zlib_POPULATED = ${zlib_POPULATED}")
message(STATUS "ZLIB_INCLUDE_DIR = ${ZLIB_INCLUDE_DIR}")
message(STATUS "ZLIB_LIBRARY = ${ZLIB_LIBRARY}")

message(STATUS "*** MAKEAVAILABLE fltk, libpng")
FetchContent_MakeAvailable(
  fltk
  libpng
)
