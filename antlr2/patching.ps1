Set-Location C:\Users\marek\source\repos\GAViewer\antlr2

##############################################################
#
#   ./lib/cpp/antlr/CharScanner.hpp
#

# Generate a patch for the CharScanner.hpp file (changes between the original and the revised file)
Copy-Item ./lib/cpp/antlr/CharScanner.hpp.revised ../build/_deps/antlr2-src/lib/cpp/antlr/CharScanner.hpp
Copy-Item ./lib/cpp/antlr/CharScanner.hpp.original ./lib/cpp/antlr/CharScanner.hpp
git diff --no-index --output=./lib/cpp/antlr/CharScanner.hpp.patch --patch ./lib/cpp/antlr/CharScanner.hpp ../build/_deps/antlr2-src/lib/cpp/antlr/CharScanner.hpp

# Applying the patch:
# - revised file saved in (suitable) source directory,
# - original file removed from patch directory
Copy-Item ./lib/cpp/antlr/CharScanner.hpp.original ./lib/cpp/antlr/CharScanner.hpp
git apply --unsafe-paths --directory=antlr2 ./lib/cpp/antlr/CharScanner.hpp.patch

##############################################################
#
#   ./lib/cpp/antlr/config.hpp
#

git diff --no-index --output=./lib/cpp/antlr/config.hpp.patch --patch ./lib/cpp/antlr/config.hpp ../build/_deps/antlr2-src/lib/cpp/antlr/config.hpp
git apply --unsafe-paths --directory=antlr2 c:/Users/marek/source/repos/GAViewer/antlr2/lib/cpp/antlr/config.hpp.patch   

##############################################################
#
#   ./lib/cpp/src/String.cpp
#

git diff --no-index --output=./lib/cpp/src/String.cpp.patch --patch ./lib/cpp/src/String.cpp ../build/_deps/antlr2-src/lib/cpp/src/String.cpp
git apply --unsafe-paths --directory=antlr2 c:/Users/marek/source/repos/GAViewer/antlr2/lib/cpp/src/String.cpp.patch
