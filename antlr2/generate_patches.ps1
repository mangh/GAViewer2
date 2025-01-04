cd C:\Users\marek\source\repos\GAViewer\antlr2
git diff --no-index --diff-filter=M ./lib/cpp ../build/_deps/antlr2-src/lib/cpp > ./antlr2.patch

git apply --unsafe-paths --directory=antlr2 ./antlr2.patch

# diff --git a/./lib/cpp/antlr/CharScanner.hpp b/../build/_deps/antlr2-src/lib/cpp/antlr/CharScanner.hpp
# index 8a97e97..49c3d82 100644
# --- a/./lib/cpp/antlr/CharScanner.hpp
# +++ b/../build/_deps/antlr2-src/lib/cpp/antlr/CharScanner.hpp
# @@ -11,6 +11,7 @@
#  #include <antlr/config.hpp>
#  
#  #include <map>
# +#include <functional>
#  
#  #ifdef HAS_NOT_CCTYPE_H
#  #include <ctype.h>
# @@ -64,7 +65,7 @@ ANTLR_C_USING(strcasecmp)
#  
#  /** Functor for the literals map
#   */
# -class ANTLR_API CharScannerLiteralsLess : public ANTLR_USE_NAMESPACE(std)binary_function<ANTLR_USE_NAMESPACE(std)string,ANTLR_USE_NAMESPACE(std)string,bool> {
# +class ANTLR_API CharScannerLiteralsLess : public ANTLR_USE_NAMESPACE(std)function<bool(ANTLR_USE_NAMESPACE(std)string,ANTLR_USE_NAMESPACE(std)string)> {
#  private:
#  	const CharScanner* scanner;
#  public:
# diff --git a/./lib/cpp/antlr/config.hpp b/../build/_deps/antlr2-src/lib/cpp/antlr/config.hpp
# index e336950..ec59f65 100644
# --- a/./lib/cpp/antlr/config.hpp
# +++ b/../build/_deps/antlr2-src/lib/cpp/antlr/config.hpp
# @@ -34,6 +34,14 @@
#  
#  #define ANTLR_ATOI_IN_STD
#  
# +/******************************************************************************/
# +/*{{{ Systems based on the Linux kernel */
# +#if defined(__linux__)
# +// The standard C library on Linux provides the strcasecmp() function
# +// for case-insensitive string comparison
# +#include <strings.h>
# +#endif
# +/*}}}*/
#  /******************************************************************************/
#  /*{{{ Microsoft Visual C++ */
#  // NOTE: If you provide patches for a specific MSVC version guard them for
# diff --git a/./lib/cpp/src/String.cpp b/../build/_deps/antlr2-src/lib/cpp/src/String.cpp
# index bb95519..e07b5fb 100644
# --- a/./lib/cpp/src/String.cpp
# +++ b/../build/_deps/antlr2-src/lib/cpp/src/String.cpp
# @@ -38,7 +38,7 @@ ANTLR_USE_NAMESPACE(std)string operator+( const ANTLR_USE_NAMESPACE(std)string&
#  ANTLR_USE_NAMESPACE(std)string operator+( const ANTLR_USE_NAMESPACE(std)string& lhs, size_t rhs )
#  {
#  	char tmp[100];
# -	sprintf(tmp,"%u",rhs);
# +	sprintf(tmp,"%zu",rhs);
#  	return lhs+tmp;
#  }
#  
# 