# Patch for GAViewer

## Purpose

The source code of ANTLR2 and GAViewer is already years old. Today's C/C++ compilers reject some code fragments as `obsolete expressions`; they also warn of the advent of `64-bit expressions`, which are still handled as if they were 32-bit. Some problems are related to the `OS platform`: some platforms were supported before, but in a different way (e.g., Linux), some were not used at all before (e.g., FreeBSD).

The purpose of the patch is to fix these errors and allow the code to compile successfully (many warnings are still displayed during compilation; some - those that seem obvious - have been disabled in the CMake script so that the more serious ones can be focused on).

## How to create a patch?

- [ ] Go to `viewer` subdirectory in the `GAViewer` repository:
  ```sh
  cd ~/GAViewer/viewer
  ```
- [ ] Save the original files you want to patch (save each file to a subdirectory with the same relative path as the original):
  ```sh
  cp ../build/_deps/gavr-src/src/gaviewer.rc ./src
  cp ../build/_deps/gavr-src/src/mainwindow.h ./src
  cp ../build/_deps/gavr-src/src/osdep.h ./src
  cp ../build/_deps/gavr-src/src/state.h ./src
  cp ../build/_deps/gavr-src/src/ui.cpp ./src
  cp ../build/_deps/gavr-src/src/c3ga/c3ga.cpp ./src/c3ga
  cp ../build/_deps/gavr-src/src/c5ga/c5ga.cpp ./src/c5ga
  cp ../build/_deps/gavr-src/src/e3ga/e3ga.cpp ./src/e3ga
  cp ../build/_deps/gavr-src/src/i2ga/i2ga.cpp ./src/i2ga
  cp ../build/_deps/gavr-src/src/p3ga/p3ga.cpp ./src/p3ga
  cp ../build/_deps/gavr-src/src/typeset/fontparse.cpp ./src/typeset
  cp ../build/_deps/gavr-src/src/typeset/fontyacc.c ./src/typeset
  cp ../build/_deps/gavr-src/src/typeset/liststack.h ./src/typeset
  cp ../build/_deps/gavr-src/src/typeset/typeset.cpp ./src/typeset
  cp ../build/_deps/gavr-src/src/typeset/typesetbox.cpp ./src/typeset
  ```
- [ ] Make (manual) corrections to the above-mentioned files (in the `../build/_deps/gavr-src/lib/cpp/*` folders).
  ```sh
  vim ../build/_deps/gavr-src/src/gaviewer.rc
  vim ../build/_deps/gavr-src/src/mainwindow.h
  vim ../build/_deps/gavr-src/src/osdep.h
  vim ../build/_deps/gavr-src/src/state.h
  vim ../build/_deps/gavr-src/src/ui.cpp
  vim ../build/_deps/gavr-src/src/c3ga/c3ga.cpp
  vim ../build/_deps/gavr-src/src/c5ga/c5ga.cpp
  vim ../build/_deps/gavr-src/src/e3ga/e3ga.cpp
  vim ../build/_deps/gavr-src/src/i2ga/i2ga.cpp
  vim ../build/_deps/gavr-src/src/p3ga/p3ga.cpp
  vim ../build/_deps/gavr-src/src/typeset/fontparse.cpp
  vim ../build/_deps/gavr-src/src/typeset/fontyacc.c
  vim ../build/_deps/gavr-src/src/typeset/liststack.h
  vim ../build/_deps/gavr-src/src/typeset/typeset.cpp
  vim ../build/_deps/gavr-src/src/typeset/typesetbox.cpp
  ```
- [ ] Generate a patch file for all the above changes:
  ```sh
  git diff --no-index --diff-filter=M ./src ../build/_deps/gavr-src/src > ./src.patch
  ```
- [ ] The `src.patch` file (created in previous step) would be sufficient as long as `CMAKE_BINARY_DIR == “build”`, but this does not have to be the case. In order to become independent of the current value of the `CMAKE_BINARY_DIR` variable, we need to take one more step, and that is to replace:
  - all occurrences of `a/./` by `a/@PATCH_SOURCE_DIR@/` and
  - all occurrences of `b/../build/_deps/gavr-src/` with `b/@PATCH_TARGET_DIR@/`.

  and save the changes to the `src.patch.in` file:

- [ ] :x: DO NOT REMOVE `./viewer/src` FOLDER WITH THE ORIGINAL FILES :heavy_exclamation_mark: They are still required as a baseline for the `git apply` command that will be called from the `viewer/CMakeLists.txt` script.

---