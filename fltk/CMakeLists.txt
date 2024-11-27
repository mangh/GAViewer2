# Fast Light Tool Kit (cross-platform C++ GUI toolkit)
#

message(STATUS "Building FLTK ...")

Include(FetchContent)

FetchContent_Declare(
  fltk
  GIT_REPOSITORY https://github.com/fltk/fltk
  GIT_TAG         master    # release-1.4.0-1	# November, 2024
  GIT_SHALLOW     TRUE

)

FetchContent_MakeAvailable(fltk)
if(TARGET fltk::fltk)
    message(STATUS "fltk::fltk target found!!!")
else()
    message(STATUS "fltk::fltk target NOT found!!!")
endif()

# FetchContent_GetProperties(fltk)
# 
# message(STATUS "fltk_BINARY_DIR = ${fltk_BINARY_DIR}")
# message(STATUS "fltk_SOURCE_DIR = ${fltk_SOURCE_DIR}")
# message(STATUS "FLTK_INCLUDEDIR = ${FLTK_INCLUDEDIR}")
# message(STATUS "FLTK_LIBRARIES = ${FLTK_LIBRARIES}")
# 
# if(fltk_POPULATED)
#   if(NOT TARGET FLToolkit::FLToolkit)
#     add_library(FLToolkit::FLToolkit UNKNOWN IMPORTED)
#     set_target_properties(FLToolkit::FLToolkit
#         PROPERTIES
#       INTERFACE_INCLUDE_DIRECTORIES "${FLTK_INCLUDEDIR}"
#       IMPORTED_LOCATION "${FLTK_LIBRARIES}"
#       IMPORTED_LINK_INTERFACE_LANGUAGES "C++"
#     )
#   endif()
# endif()
