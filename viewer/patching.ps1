Set-Location C:\Users\marek\source\repos\GAViewer\viewer

##############################################################
#
#   ./src/gaviewer.rc
#

# Generate a patch for the gaviewer.rc file (changes between the original and the patched file)
Copy-Item ./src/gaviewer.rc.patched ../build/_deps/gavr-src/src/gaviewer.rc
Copy-Item ./src/gaviewer.rc.original ./src/gaviewer.rc
git diff --no-index --output=./src/gaviewer.rc.patch --patch ./src/gaviewer.rc ../build/_deps/gavr-src/src/gaviewer.rc

##############################################################
#
#   ./src/mainwindow.h
#

git diff --no-index --output=./src/mainwindow.h.patch --patch ./src/mainwindow.h ../build/_deps/gavr-src/src/mainwindow.h

##############################################################
#
#   ./src/osdep.h
#

git diff --no-index --output=./src/osdep.h.patch --patch ./src/osdep.h ../build/_deps/gavr-src/src/osdep.h

git diff --no-index --output=./src/state.h.patch --patch ./src/state.h ../build/_deps/gavr-src/src/state.h

git diff --no-index --output=./src/ui.cpp.patch --patch ./src/ui.cpp ../build/_deps/gavr-src/src/ui.cpp

git diff --no-index --output=./src/c3ga/c3ga.cpp.patch --patch ./src/c3ga/c3ga.cpp ../build/_deps/gavr-src/src/c3ga/c3ga.cpp
warning: in the working copy of 'viewer/./src/c3ga/c3ga.cpp', LF will be replaced by CRLF the next time Git touches it
warning: in the working copy of 'viewer/../build/_deps/gavr-src/src/c3ga/c3ga.cpp', LF will be replaced by CRLF the next time Git touches it

git diff --no-index --output=./src/c5ga/c5ga.cpp.patch --patch ./src/c5ga/c5ga.cpp ../build/_deps/gavr-src/src/c5ga/c5ga.cpp
warning: in the working copy of 'viewer/./src/c5ga/c5ga.cpp', LF will be replaced by CRLF the next time Git touches it
warning: in the working copy of 'viewer/../build/_deps/gavr-src/src/c5ga/c5ga.cpp', LF will be replaced by CRLF the next time Git touches it

git diff --no-index --output=./src/e3ga/e3ga.cpp.patch --patch ./src/e3ga/e3ga.cpp ../build/_deps/gavr-src/src/e3ga/e3ga.cpp
warning: in the working copy of 'viewer/./src/e3ga/e3ga.cpp', LF will be replaced by CRLF the next time Git touches it
warning: in the working copy of 'viewer/../build/_deps/gavr-src/src/e3ga/e3ga.cpp', LF will be replaced by CRLF the next time Git touches it

git diff --no-index --output=./src/i2ga/i2ga.cpp.patch --patch ./src/i2ga/i2ga.cpp ../build/_deps/gavr-src/src/i2ga/i2ga.cpp

git diff --no-index --output=./src/p3ga/p3ga.cpp.patch --patch ./src/p3ga/p3ga.cpp ../build/_deps/gavr-src/src/p3ga/p3ga.cpp

git diff --no-index --output=./src/typeset/fontparse.cpp.patch --patch ./src/typeset/fontparse.cpp ../build/_deps/gavr-src/src/typeset/fontparse.cpp

git diff --no-index --output=./src/typeset/fontyacc.c.patch --patch ./src/typeset/fontyacc.c ../build/_deps/gavr-src/src/typeset/fontyacc.c

git diff --no-index --output=./src/typeset/liststack.h.patch --patch ./src/typeset/liststack.h ../build/_deps/gavr-src/src/typeset/liststack.h

git diff --no-index --output=./src/typeset/typeset.cpp.patch --patch ./src/typeset/typeset.cpp ../build/_deps/gavr-src/src/typeset/typeset.cpp

git diff --no-index --output=./src/typeset/typesetbox.cpp.patch --patch ./src/typeset/typesetbox.cpp ../build/_deps/gavr-src/src/typeset/typesetbox.cpp