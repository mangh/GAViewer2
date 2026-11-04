# Patch for ANTLR2

## Purpose

The source code of ANTLR2 and GAViewer is already years old. Today's C/C++ compilers reject some code fragments as `obsolete expressions`; they also warn of the advent of `64-bit expressions`, which are still handled as if they were 32-bit. Some problems are related to the `OS platform`: some platforms were supported before, but in a different way (e.g., Linux), some were not used at all before (e.g., FreeBSD). Some standard library features used by ANTLR2 have been removed from newer `C++ standards` (e.g., `std::binary_function` in C++17, `std::uncaught_exception` in C++20).

The purpose of the patch is to fix these errors and allow the code to compile successfully (many warnings are still displayed during compilation; some - those that seem obvious - have been disabled in the CMake script so that the more serious ones can be focused on).

## How to create a patch?

- [ ] Go to `antlr2` subdirectory in the `GAViewer` repository:
  ```sh
  cd ~/GAViewer/antlr2
  ```
- [ ] Save the original files you want to patch (save each file to a subdirectory with the same relative path as the original):
  ```sh
  cp ../build/_deps/antlr2-src/lib/cpp/antlr/CharScanner.hpp ./lib/cpp/antlr
  cp ../build/_deps/antlr2-src/lib/cpp/antlr/config.hpp ./lib/cpp/antlr
  cp ../build/_deps/antlr2-src/lib/cpp/antlr/Parser.hpp ./lib/cpp/antlr
  cp ../build/_deps/antlr2-src/lib/cpp/src/String.cpp ./lib/cpp/src
  ```
- [ ] Add each new file to the `files_to_patch` list in the `antlr2/CMakeLists.txt` script (so that the patch is re-applied whenever it changes):
  ```cmake
  set(files_to_patch
    "${ANTLR_LIB_INCLUDE_PATH}/CharScanner.hpp"
    "${ANTLR_LIB_INCLUDE_PATH}/config.hpp"
    "${ANTLR_LIB_INCLUDE_PATH}/Parser.hpp"
    "${ANTLR_LIB_SOURCE_PATH}/String.cpp"
  )
  ```
- [ ] Make (manual) corrections to the above-mentioned files (in the `../build/_deps/antlr2-src/lib/cpp/*` folders).
  ```sh
  vim ../build/_deps/antlr2-src/lib/cpp/antlr/CharScanner.hpp
  vim ../build/_deps/antlr2-src/lib/cpp/antlr/config.hpp
  vim ../build/_deps/antlr2-src/lib/cpp/antlr/Parser.hpp
  vim ../build/_deps/antlr2-src/lib/cpp/src/String.cpp
  ```
- [ ] Generate a patch file for all the above changes:
  ```sh
  git diff --no-index --diff-filter=M ./lib/cpp ../build/_deps/antlr2-src/lib/cpp > ./lib.patch
  ```
  This creates the follwing `lib.patch` file:
  ```diff
  diff --git a/./lib/cpp/antlr/CharScanner.hpp b/../build/_deps/antlr2-src/lib/cpp/antlr/CharScanner.hpp
  index 8a97e97..49c3d82 100644
  --- a/./lib/cpp/antlr/CharScanner.hpp
  +++ b/../build/_deps/antlr2-src/lib/cpp/antlr/CharScanner.hpp
  @@ -11,6 +11,7 @@
   #include <antlr/config.hpp>
   
   #include <map>
  +#include <functional>
   
   #ifdef HAS_NOT_CCTYPE_H
   #include <ctype.h>
  @@ -64,7 +65,7 @@ ANTLR_C_USING(strcasecmp)
   
   /** Functor for the literals map
    */
  -class ANTLR_API CharScannerLiteralsLess : public ANTLR_USE_NAMESPACE(std)binary_function<ANTLR_USE_NAMESPACE(std)  string,ANTLR_USE_NAMESPACE(std)string,bool> {
  +class ANTLR_API CharScannerLiteralsLess : public ANTLR_USE_NAMESPACE(std)function<bool(ANTLR_USE_NAMESPACE(std)  string,ANTLR_USE_NAMESPACE(std)string)> {
   private:
   	const CharScanner* scanner;
   public:
  diff --git a/./lib/cpp/antlr/Parser.hpp b/../build/_deps/antlr2-src/lib/cpp/antlr/Parser.hpp
  index c078ec6..e87881e 100644
  --- a/./lib/cpp/antlr/Parser.hpp
  +++ b/../build/_deps/antlr2-src/lib/cpp/antlr/Parser.hpp
  @@ -298,7 +298,7 @@ protected:
   		{
   #ifdef ANTLR_CXX_SUPPORTS_UNCAUGHT_EXCEPTION
   			// Only give trace if there's no uncaught exception..
  -			if(!ANTLR_USE_NAMESPACE(std)uncaught_exception())
  +			if(ANTLR_USE_NAMESPACE(std)uncaught_exceptions() == 0)
   #endif
   				parser->traceOut(text);
   		}
  diff --git a/./lib/cpp/antlr/config.hpp b/../build/_deps/antlr2-src/lib/cpp/antlr/config.hpp
  index e336950..ec59f65 100644
  --- a/./lib/cpp/antlr/config.hpp
  +++ b/../build/_deps/antlr2-src/lib/cpp/antlr/config.hpp
  @@ -34,6 +34,14 @@
   
   #define ANTLR_ATOI_IN_STD
   
  +/******************************************************************************/
  +/*{{{ Systems based on the Linux kernel */
  +#if defined(__linux__)
  +// The standard C library on Linux provides the strcasecmp() function
  +// for case-insensitive string comparison
  +#include <strings.h>
  +#endif
  +/*}}}*/
   /******************************************************************************/
   /*{{{ Microsoft Visual C++ */
   // NOTE: If you provide patches for a specific MSVC version guard them for
  diff --git a/./lib/cpp/src/String.cpp b/../build/_deps/antlr2-src/lib/cpp/src/String.cpp
  index bb95519..e07b5fb 100644
  --- a/./lib/cpp/src/String.cpp
  +++ b/../build/_deps/antlr2-src/lib/cpp/src/String.cpp
  @@ -38,7 +38,7 @@ ANTLR_USE_NAMESPACE(std)string operator+( const ANTLR_USE_NAMESPACE(std)string&
   ANTLR_USE_NAMESPACE(std)string operator+( const ANTLR_USE_NAMESPACE(std)string& lhs, size_t rhs )
   {
   	char tmp[100];
  -	sprintf(tmp,"%u",rhs);
  +	sprintf(tmp,"%zu",rhs);
   	return lhs+tmp;
   }
   
  ```
- [ ] The above patch file would be sufficient as long as `CMAKE_BINARY_DIR == “build”`, but this does not have to be the case. In order to become independent of the current value of the `CMAKE_BINARY_DIR` variable, we need to take one more step, and that is to replace:
  - all occurrences of `a/./` by `a/@PATCH_SOURCE_DIR@/` and
  - all occurrences of `b/../build/_deps/antlr2-src/` with `b/@PATCH_TARGET_DIR@/`.

  and save the changes to the `lib.patch.in` file:
  ```diff
  diff --git a/@PATCH_SOURCE_DIR@/lib/cpp/antlr/CharScanner.hpp b/@PATCH_TARGET_DIR@/lib/cpp/antlr/CharScanner.hpp
  index 8a97e97..49c3d82 100644
  --- a/@PATCH_SOURCE_DIR@/lib/cpp/antlr/CharScanner.hpp
  +++ b/@PATCH_TARGET_DIR@/lib/cpp/antlr/CharScanner.hpp
  @@ -11,6 +11,7 @@
   #include <antlr/config.hpp>
   
   #include <map>
  +#include <functional>
   
   #ifdef HAS_NOT_CCTYPE_H
   #include <ctype.h>
  @@ -64,7 +65,7 @@ ANTLR_C_USING(strcasecmp)
   
   /** Functor for the literals map
    */
  -class ANTLR_API CharScannerLiteralsLess : public ANTLR_USE_NAMESPACE(std)binary_function<ANTLR_USE_NAMESPACE(std)  string,ANTLR_USE_NAMESPACE(std)string,bool> {
  +class ANTLR_API CharScannerLiteralsLess : public ANTLR_USE_NAMESPACE(std)function<bool(ANTLR_USE_NAMESPACE(std)  string,ANTLR_USE_NAMESPACE(std)string)> {
   private:
   	const CharScanner* scanner;
   public:
  diff --git a/@PATCH_SOURCE_DIR@/lib/cpp/antlr/Parser.hpp b/@PATCH_TARGET_DIR@/lib/cpp/antlr/Parser.hpp
  index c078ec6..e87881e 100644
  --- a/@PATCH_SOURCE_DIR@/lib/cpp/antlr/Parser.hpp
  +++ b/@PATCH_TARGET_DIR@/lib/cpp/antlr/Parser.hpp
  @@ -298,7 +298,7 @@ protected:
   		{
   #ifdef ANTLR_CXX_SUPPORTS_UNCAUGHT_EXCEPTION
   			// Only give trace if there's no uncaught exception..
  -			if(!ANTLR_USE_NAMESPACE(std)uncaught_exception())
  +			if(ANTLR_USE_NAMESPACE(std)uncaught_exceptions() == 0)
   #endif
   				parser->traceOut(text);
   		}
  diff --git a/@PATCH_SOURCE_DIR@/lib/cpp/antlr/config.hpp b/@PATCH_TARGET_DIR@/lib/cpp/antlr/config.hpp
  index e336950..ec59f65 100644
  --- a/@PATCH_SOURCE_DIR@/lib/cpp/antlr/config.hpp
  +++ b/@PATCH_TARGET_DIR@/lib/cpp/antlr/config.hpp
  @@ -34,6 +34,14 @@
   
   #define ANTLR_ATOI_IN_STD
   
  +/******************************************************************************/
  +/*{{{ Systems based on the Linux kernel */
  +#if defined(__linux__)
  +// The standard C library on Linux provides the strcasecmp() function
  +// for case-insensitive string comparison
  +#include <strings.h>
  +#endif
  +/*}}}*/
   /******************************************************************************/
   /*{{{ Microsoft Visual C++ */
   // NOTE: If you provide patches for a specific MSVC version guard them for
  diff --git a/@PATCH_SOURCE_DIR@/lib/cpp/src/String.cpp b/@PATCH_TARGET_DIR@/lib/cpp/src/String.cpp
  index bb95519..e07b5fb 100644
  --- a/@PATCH_SOURCE_DIR@/lib/cpp/src/String.cpp
  +++ b/@PATCH_TARGET_DIR@/lib/cpp/src/String.cpp
  @@ -38,7 +38,7 @@ ANTLR_USE_NAMESPACE(std)string operator+( const ANTLR_USE_NAMESPACE(std)string&
   ANTLR_USE_NAMESPACE(std)string operator+( const ANTLR_USE_NAMESPACE(std)string& lhs, size_t rhs )
   {
   	char tmp[100];
  -	sprintf(tmp,"%u",rhs);
  +	sprintf(tmp,"%zu",rhs);
   	return lhs+tmp;
   }
   
  ```

- [ ] :x: DO NOT REMOVE `./antlr2/lib` FOLDER WITH THE ORIGINAL FILES :heavy_exclamation_mark: They are still required as a baseline for the `git apply` command that will be called from the `antlr2/CMakeLists.txt` script.

---