;;; Directory Local Variables            -*- no-byte-compile: t -*-
;;; For more information see (info "(emacs) Directory Variables")

((nil . ((compile-command . "cmake --build ~/GAViewer/build --clean-first")))
  (c++-mode . (
    (ff-search-directories .
      ("."
       "~/GAViewer/build/_deps/antlr2-src/lib/cpp/antlr/"
       "~/GAViewer/build/_deps/fltk-build/"
       "~/GAViewer/build/_deps/fltk-src/"
       "~/GAViewer/build/_deps/fltk-src/fluid/"
       "~/GAViewer/build/_deps/fltk-src/GL/"
       "~/GAViewer/build/_deps/fltk-src/jpeg/"
       "~/GAViewer/build/_deps/fltk-src/png/"
       "~/GAViewer/build/_deps/fltk-src/src/"
       "~/GAViewer/build/_deps/fltk-src/zlib/"
       "~/GAViewer/build/_deps/gavr-src/src/"
       "~/GAViewer/build/_deps/gavr-src/src/console/"
       "~/GAViewer/build/_deps/gavr-src/src/typeset/"
       "/usr/local/include"
       ))))
  (cmake-mode . (
    (compile-command . "cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=On --log-level=DEBUG -S ~/GAViewer -B ~/GAViewer/build --fresh"))))
