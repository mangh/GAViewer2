# Components for building ANTLR library

* `lib` - subdirectory with the original ANTLR2 files (but only those that need to be fixed/patched before building a working library; note that each file is saved in a subdirectory with THE SAME RELATIVE PATH as the original).
* `lib.patch.in` - patch file as a CMake template, from which the actual patch file is generated (taking into account the current location of the CMAKE_BINARY_DIR directory to which ANTLR2 source files are downloaded). For more information on creating this template, see [Patch_File](patch_file.md) document. 
* `CMakeLists.txt` - CMake script that downloads, patches and builds the ANTLR2 library.

---