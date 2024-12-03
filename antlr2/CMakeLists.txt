# ANTLR v2 stable version is 2.7.7 (released November 1, 2006)
#

message(DEBUG "Entering the ANTLR2 subdirectory ...")

##########################################################################
#
#   PROJECT
#

set(ANTLR_VERSION_MAJOR 2)
set(ANTLR_VERSION_MINOR 7)
set(ANTLR_VERSION_PATCH 7)

set(ANTLR_VERSION "${ANTLR_VERSION_MAJOR}.${ANTLR_VERSION_MINOR}.${ANTLR_VERSION_PATCH}")

project(antlr
  VERSION "${ANTLR_VERSION}"
  LANGUAGES C CXX
  DESCRIPTION "ANTLR, (AN)other (T)ool for (L)anguage (R)ecognition"
)

# Naming:
set(ANTLR_LIB_VER "antlr${ANTLR_VERSION_MAJOR}${ANTLR_VERSION_MINOR}")
set(ANTLR_LIB_VER_CFG "${ANTLR_LIB_VER}$<$<CONFIG:Debug>:d>")
# Path "${CMAKE_SOURCE_DIR}/antlr2/antlr27[d]?.{lib|a}"
set(ANTLR_LIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/${ANTLR_LIB_VER}")

##########################################################################
#
#	  Downloading source files
#
#   Name: antlr-2.7.7.tar.gz
#   Size: 1816180 bytes : 1773 KiB
#   SHA256: 853aeb021aef7586bda29e74a6b03006bcb565a755c86b66032d8ec31b67dbb9
#

Include(FetchContent)

FetchContent_Declare(
  antlr2
  URL https://www.antlr2.org/download/antlr-2.7.7.tar.gz
  URL_HASH SHA256=853aeb021aef7586bda29e74a6b03006bcb565a755c86b66032d8ec31b67dbb9
)

FetchContent_MakeAvailable(antlr2)

FetchContent_GetProperties(antlr2)

message(DEBUG "antlr2_POPULATED = ${antlr2_POPULATED}")
message(DEBUG "antlr2_SOURCE_DIR = ${antlr2_SOURCE_DIR}")
message(DEBUG "antlr2_BINARY_DIR = ${antlr2_BINARY_DIR}")

cmake_path(SET ANTLR_TOP_DIR "${antlr2_SOURCE_DIR}/lib/cpp")
cmake_path(SET ANTLR_INC_DIR "${ANTLR_TOP_DIR}/antlr")
cmake_path(SET ANTLR_SRC_DIR "${ANTLR_TOP_DIR}/src")

##########################################################################
#
#	  Patching
#
#

include(Patch)
patch(
  "${antlr2_SOURCE_DIR}"          # source directory
  "lib/cpp/antlr/CharScanner.hpp" # name of the file to patch
  "^#include <functional>$"       # what should be in the patched (revised) file
  "${CMAKE_CURRENT_SOURCE_DIR}"   # patch directory containing finished, patched files
)

patch(
  "${antlr2_SOURCE_DIR}"          # source directory
  "lib/cpp/src/String.cpp"        # name of the file to patch
  "^[ \t]+sprintf\\(tmp,\"%zu\",rhs\\)\;$"  # what should be in the patched (revised) file
  "${CMAKE_CURRENT_SOURCE_DIR}"   # patch directory containing finished, patched files
)

##########################################################################
#
#	  ANTLR library (target to build)
#

set(target ANTLR_LIB)

# # antlr headers: ls ${ANTLR_INC_DIR}/*.hpp
# set(ANTLR_PUBLIC_HEADERS
#   ${ANTLR_INC_DIR}/ANTLRException.hpp
#   ${ANTLR_INC_DIR}/ANTLRUtil.hpp
#   ${ANTLR_INC_DIR}/AST.hpp
#   ${ANTLR_INC_DIR}/ASTArray.hpp
#   ${ANTLR_INC_DIR}/ASTFactory.hpp
#   ${ANTLR_INC_DIR}/ASTNULLType.hpp
#   ${ANTLR_INC_DIR}/ASTPair.hpp
#   ${ANTLR_INC_DIR}/ASTRefCount.hpp
#   ${ANTLR_INC_DIR}/BaseAST.hpp
#   ${ANTLR_INC_DIR}/BitSet.hpp
#   ${ANTLR_INC_DIR}/CharBuffer.hpp
#   ${ANTLR_INC_DIR}/CharInputBuffer.hpp
#   ${ANTLR_INC_DIR}/CharScanner.hpp
#   ${ANTLR_INC_DIR}/CharStreamException.hpp
#   ${ANTLR_INC_DIR}/CharStreamIOException.hpp
#   ${ANTLR_INC_DIR}/CircularQueue.hpp
#   ${ANTLR_INC_DIR}/CommonAST.hpp
#   ${ANTLR_INC_DIR}/CommonASTWithHiddenTokens.hpp
#   ${ANTLR_INC_DIR}/CommonHiddenStreamToken.hpp
#   ${ANTLR_INC_DIR}/CommonToken.hpp
#   ${ANTLR_INC_DIR}/config.hpp
#   ${ANTLR_INC_DIR}/InputBuffer.hpp
#   ${ANTLR_INC_DIR}/IOException.hpp
#   ${ANTLR_INC_DIR}/LexerSharedInputState.hpp
#   ${ANTLR_INC_DIR}/LLkParser.hpp
#   ${ANTLR_INC_DIR}/MismatchedCharException.hpp
#   ${ANTLR_INC_DIR}/MismatchedTokenException.hpp
#   ${ANTLR_INC_DIR}/NoViableAltException.hpp
#   ${ANTLR_INC_DIR}/NoViableAltForCharException.hpp
#   ${ANTLR_INC_DIR}/Parser.hpp
#   ${ANTLR_INC_DIR}/ParserSharedInputState.hpp
#   ${ANTLR_INC_DIR}/RecognitionException.hpp
#   ${ANTLR_INC_DIR}/RefCount.hpp
#   ${ANTLR_INC_DIR}/SemanticException.hpp
#   ${ANTLR_INC_DIR}/String.hpp
#   ${ANTLR_INC_DIR}/Token.hpp
#   ${ANTLR_INC_DIR}/TokenBuffer.hpp
#   ${ANTLR_INC_DIR}/TokenRefCount.hpp
#   ${ANTLR_INC_DIR}/TokenStream.hpp
#   ${ANTLR_INC_DIR}/TokenStreamBasicFilter.hpp
#   ${ANTLR_INC_DIR}/TokenStreamException.hpp
#   ${ANTLR_INC_DIR}/TokenStreamHiddenTokenFilter.hpp
#   ${ANTLR_INC_DIR}/TokenStreamIOException.hpp
#   ${ANTLR_INC_DIR}/TokenStreamRecognitionException.hpp
#   ${ANTLR_INC_DIR}/TokenStreamRetryException.hpp
#   ${ANTLR_INC_DIR}/TokenStreamRewriteEngine.hpp
#   ${ANTLR_INC_DIR}/TokenStreamSelector.hpp
#   ${ANTLR_INC_DIR}/TokenWithIndex.hpp
#   ${ANTLR_INC_DIR}/TreeParser.hpp
#   ${ANTLR_INC_DIR}/TreeParserSharedInputState.hpp
# )

# antlr library source: ls ${ANTLR_SRC_DIR}/*.cpp (exclude: dll.cpp)
add_library(${target}
  ${ANTLR_SRC_DIR}/ANTLRUtil.cpp
  ${ANTLR_SRC_DIR}/ASTFactory.cpp
  ${ANTLR_SRC_DIR}/ASTNULLType.cpp
  ${ANTLR_SRC_DIR}/ASTRefCount.cpp
  ${ANTLR_SRC_DIR}/BaseAST.cpp
  ${ANTLR_SRC_DIR}/BitSet.cpp
  ${ANTLR_SRC_DIR}/CharBuffer.cpp
  ${ANTLR_SRC_DIR}/CharScanner.cpp
  ${ANTLR_SRC_DIR}/CommonAST.cpp
  ${ANTLR_SRC_DIR}/CommonASTWithHiddenTokens.cpp
  ${ANTLR_SRC_DIR}/CommonHiddenStreamToken.cpp
  ${ANTLR_SRC_DIR}/CommonToken.cpp
#  ${ANTLR_SRC_DIR}/dll.cpp
  ${ANTLR_SRC_DIR}/InputBuffer.cpp
  ${ANTLR_SRC_DIR}/LLkParser.cpp
  ${ANTLR_SRC_DIR}/MismatchedCharException.cpp
  ${ANTLR_SRC_DIR}/MismatchedTokenException.cpp
  ${ANTLR_SRC_DIR}/NoViableAltException.cpp
  ${ANTLR_SRC_DIR}/NoViableAltForCharException.cpp
  ${ANTLR_SRC_DIR}/Parser.cpp
  ${ANTLR_SRC_DIR}/RecognitionException.cpp
  ${ANTLR_SRC_DIR}/String.cpp
  ${ANTLR_SRC_DIR}/Token.cpp
  ${ANTLR_SRC_DIR}/TokenBuffer.cpp
  ${ANTLR_SRC_DIR}/TokenRefCount.cpp
  ${ANTLR_SRC_DIR}/TokenStreamBasicFilter.cpp
  ${ANTLR_SRC_DIR}/TokenStreamHiddenTokenFilter.cpp
  ${ANTLR_SRC_DIR}/TokenStreamRewriteEngine.cpp
  ${ANTLR_SRC_DIR}/TokenStreamSelector.cpp
  ${ANTLR_SRC_DIR}/TreeParser.cpp
)

set_target_properties(${target}
    PROPERTIES 
#  PUBLIC_HEADER "${ANTLR_PUBLIC_HEADERS}"
  OUTPUT_NAME "${ANTLR_LIB_VER_CFG}"
)

##########################################################################
#
#   Compile & Link Options
#

target_compile_options(${target} PRIVATE $<IF:$<STREQUAL:${CMAKE_CXX_COMPILER_FRONTEND_VARIANT},MSVC>,/W3 /wd4996,-Wall>) # but not -Wextra
target_include_directories(${target} PUBLIC "${ANTLR_TOP_DIR}")

##########################################################################
#
#   Antlr2 as a CMake target (to communicate
#   the Antlr2 Usage Requirements to importers)
#

if(antlr2_POPULATED)
  if(NOT TARGET antlr2::antlr2)
    add_library(antlr2::antlr2 UNKNOWN IMPORTED GLOBAL)
    set_target_properties(antlr2::antlr2
        PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${ANTLR_TOP_DIR}"
#      IMPORTED_LOCATION "c:/Users/marek/source/repos/GAViewer/out/build/x86-Debug/antlr2/antlr27d.lib"
      IMPORTED_LOCATION "${ANTLR_LIB_PATH}${CMAKE_LINK_LIBRARY_SUFFIX}"
      IMPORTED_LOCATION_DEBUG "${ANTLR_LIB_PATH}d${CMAKE_LINK_LIBRARY_SUFFIX}"
      IMPORTED_LINK_INTERFACE_LANGUAGES "C++"
    )
    # include(DebugTargetProperties OPTIONAL RESULT_VARIABLE DebugTargetProperties_FOUND)
    # if (DebugTargetProperties_FOUND)
    #   target_properties_report("antlr2::antlr2" TARGET_IMPORTED_PROPERTIES TARGET_INTERFACE_PROPERTIES)
    # endif()
  endif()
endif()
