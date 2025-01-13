# Patch for FLTK

## Purpose

This patch is for `FLTK version 1.4.1` (released on December 12, 2024). It enables successful compilation of the `FLUID` application for the `X11` backend on `BSD` systems (without it, a FLUID compilation error occurs, which will consequently abort FLTK compilation). Subsequent versions of FLTK may not need the patch.

## How to create a patch?

- [ ] Go to `fltk-src` subdirectory in the `GAViewer/build/_deps` repository:
  ```sh
  cd ~/GAViewer/build/_deps/fltk-src
  ```
- [ ] Make (manual) corrections to the original `./fluid/CMakeLists.txt` file:
  ```sh
  vim ../fluid/CMakeLists.txt
  ```
- [ ] Generate a patch file for the changes made above:
  ```sh
  git diff > ~/GAViewer/fltk/fltk.patch
  ```
  This should create the follwing `fltk.patch` file:
  ```diff
  diff --git a/fluid/CMakeLists.txt b/fluid/CMakeLists.txt
  index b906561..1cbafa7 100644
  --- a/fluid/CMakeLists.txt
  +++ b/fluid/CMakeLists.txt
  @@ -109,6 +109,9 @@ source_group("Header Files" FILES ${HEADERFILES})
   add_library(fluid-lib OBJECT EXCLUDE_FROM_ALL)
   target_sources(fluid-lib PRIVATE ${CPPFILES} ${HEADERFILES})
   target_link_libraries(fluid-lib PUBLIC fltk::images)
  +if(FLTK_BACKEND_X11)
  +  target_include_directories(fluid-lib PRIVATE ${X11_INCLUDE_DIR})
  +endif()
   
   # Build targets
   
  ```
---

