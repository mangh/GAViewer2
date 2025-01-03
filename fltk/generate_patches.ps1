##########################################################################
#
#   Generate patches for FLTK
#   using the capabilities of the Git repository.
#
#   After downloading FLTK make required changes and then do the following:
#
#     cd ${CMAKE_SOURCE_DIR}/build/_deps/fltk-src
#     git diff > ${CMAKE_SOURCE_DIR}/fltk/fltk.patch
#     -- or --
#     git diff --output=${CMAKE_SOURCE_DIR}/fltk/fltk.patch
#
#   For example:
#

cd C:\Users\marek\source\repos\GAViewer\build\_deps\fltk-src
git diff > C:\Users\marek\source\repos\GAViewer\fltk\fltk.patch

# diff --git a/fluid/CMakeLists.txt b/fluid/CMakeLists.txt
# index b906561..1cbafa7 100644
# --- a/fluid/CMakeLists.txt
# +++ b/fluid/CMakeLists.txt
# @@ -109,6 +109,9 @@ source_group("Header Files" FILES ${HEADERFILES})
#  add_library(fluid-lib OBJECT EXCLUDE_FROM_ALL)
#  target_sources(fluid-lib PRIVATE ${CPPFILES} ${HEADERFILES})
#  target_link_libraries(fluid-lib PUBLIC fltk::images)
# +if(FLTK_BACKEND_X11)
# +  target_include_directories(fluid-lib PRIVATE ${X11_INCLUDE_DIR})
# +endif()
# 
#  # Build targets
# 
#

#####################################################################
#
#   Generate patches for FLTK
#   as if it was not Git repository
#

# cd C:\Users\marek\source\repos\GAViewer\fltk
# git diff --no-index --output=./fluid/CMakeLists.txt.patch --patch ./fluid/CMakeLists.txt ../build/_deps/fltk-src/fluid/CMakeLists.txt

