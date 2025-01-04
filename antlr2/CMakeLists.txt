# ANTLR library version 2.7.7 (November 1, 2006)
#

message(DEBUG "Entering the ANTLR2 subdirectory ...")

##########################################################################
#
#   ANTLR2 directory compile options
#
#   When compiling ANTLR2 source code (which is years old), 
#   many deprecation warnings appear. It's worth turning them off to get
#   a clearer picture of the compilation and spot more serious issues (if any).
#

if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC")

  add_compile_options(
    -D_CRT_SECURE_NO_WARNINGS
    -W3     # production quality warnings
  )

elseif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "GNU")

  add_compile_options(
    -Wall
    -Wno-deprecated
  )

endif()

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
set(ANTLR_LIB_NAME "antlr${ANTLR_VERSION_MAJOR}${ANTLR_VERSION_MINOR}")
if (CMAKE_BUILD_TYPE MATCHES "Debug")
  set(ANTLR_LIB_NAME "${ANTLR_LIB_NAME}d")
endif()

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

# message(DEBUG "antlr2_POPULATED = ${antlr2_POPULATED}")
# message(DEBUG "antlr2_SOURCE_DIR = ${antlr2_SOURCE_DIR}")
# message(DEBUG "antlr2_BINARY_DIR = ${antlr2_BINARY_DIR}")

cmake_path(SET ANTLR_LIB_PATH "${antlr2_SOURCE_DIR}/lib/cpp")
cmake_path(SET ANTLR_LIB_INCLUDE_PATH "${ANTLR_LIB_PATH}/antlr")
cmake_path(SET ANTLR_LIB_SOURCE_PATH "${ANTLR_LIB_PATH}/src")

##########################################################################
#
#	  Patching
#
#

# Get value of the "--directory" option to be used in the "git apply" command:
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH _parent_dir)
string(REPLACE "${_parent_dir}" "" _slash_subdir ${CMAKE_CURRENT_SOURCE_DIR})
string(REPLACE "/" "" _subdir ${_slash_subdir})

add_custom_command(
  OUTPUT
    "${ANTLR_LIB_INCLUDE_PATH}/CharScanner.hpp"
    "${ANTLR_LIB_INCLUDE_PATH}/config.hpp"
    "${ANTLR_LIB_SOURCE_PATH}/String.cpp"
  COMMAND
    ${GIT_EXECUTABLE} apply --unsafe-paths --directory=${_subdir} "${CMAKE_CURRENT_SOURCE_DIR}/antlr2.patch"
  DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/antlr2.patch"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  COMMENT "Patch the source files in the ${ANTLR_LIB_PATH} folder."
  USES_TERMINAL
)

##########################################################################
#
#	  ANTLR library (target to build)
#

set(target ANTLR_LIB)

# antlr headers: ls ${ANTLR_LIB_INCLUDE_PATH}/*.hpp
set(ANTLR_PUBLIC_HEADERS
  ${ANTLR_LIB_INCLUDE_PATH}/ANTLRException.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/ANTLRUtil.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/AST.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/ASTArray.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/ASTFactory.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/ASTNULLType.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/ASTPair.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/ASTRefCount.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/BaseAST.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/BitSet.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/CharBuffer.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/CharInputBuffer.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/CharScanner.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/CharStreamException.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/CharStreamIOException.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/CircularQueue.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/CommonAST.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/CommonASTWithHiddenTokens.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/CommonHiddenStreamToken.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/CommonToken.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/config.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/InputBuffer.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/IOException.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/LexerSharedInputState.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/LLkParser.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/MismatchedCharException.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/MismatchedTokenException.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/NoViableAltException.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/NoViableAltForCharException.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/Parser.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/ParserSharedInputState.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/RecognitionException.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/RefCount.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/SemanticException.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/String.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/Token.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/TokenBuffer.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/TokenRefCount.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/TokenStream.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/TokenStreamBasicFilter.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/TokenStreamException.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/TokenStreamHiddenTokenFilter.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/TokenStreamIOException.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/TokenStreamRecognitionException.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/TokenStreamRetryException.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/TokenStreamRewriteEngine.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/TokenStreamSelector.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/TokenWithIndex.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/TreeParser.hpp
  ${ANTLR_LIB_INCLUDE_PATH}/TreeParserSharedInputState.hpp
)

# antlr library source: ls ${ANTLR_LIB_SOURCE_PATH}/*.cpp (exclude: dll.cpp)
add_library(${target}
  ${ANTLR_LIB_SOURCE_PATH}/ANTLRUtil.cpp
  ${ANTLR_LIB_SOURCE_PATH}/ASTFactory.cpp
  ${ANTLR_LIB_SOURCE_PATH}/ASTNULLType.cpp
  ${ANTLR_LIB_SOURCE_PATH}/ASTRefCount.cpp
  ${ANTLR_LIB_SOURCE_PATH}/BaseAST.cpp
  ${ANTLR_LIB_SOURCE_PATH}/BitSet.cpp
  ${ANTLR_LIB_SOURCE_PATH}/CharBuffer.cpp
  ${ANTLR_LIB_SOURCE_PATH}/CharScanner.cpp
  ${ANTLR_LIB_SOURCE_PATH}/CommonAST.cpp
  ${ANTLR_LIB_SOURCE_PATH}/CommonASTWithHiddenTokens.cpp
  ${ANTLR_LIB_SOURCE_PATH}/CommonHiddenStreamToken.cpp
  ${ANTLR_LIB_SOURCE_PATH}/CommonToken.cpp
#  ${ANTLR_LIB_SOURCE_PATH}/dll.cpp
  ${ANTLR_LIB_SOURCE_PATH}/InputBuffer.cpp
  ${ANTLR_LIB_SOURCE_PATH}/LLkParser.cpp
  ${ANTLR_LIB_SOURCE_PATH}/MismatchedCharException.cpp
  ${ANTLR_LIB_SOURCE_PATH}/MismatchedTokenException.cpp
  ${ANTLR_LIB_SOURCE_PATH}/NoViableAltException.cpp
  ${ANTLR_LIB_SOURCE_PATH}/NoViableAltForCharException.cpp
  ${ANTLR_LIB_SOURCE_PATH}/Parser.cpp
  ${ANTLR_LIB_SOURCE_PATH}/RecognitionException.cpp
  ${ANTLR_LIB_SOURCE_PATH}/String.cpp
  ${ANTLR_LIB_SOURCE_PATH}/Token.cpp
  ${ANTLR_LIB_SOURCE_PATH}/TokenBuffer.cpp
  ${ANTLR_LIB_SOURCE_PATH}/TokenRefCount.cpp
  ${ANTLR_LIB_SOURCE_PATH}/TokenStreamBasicFilter.cpp
  ${ANTLR_LIB_SOURCE_PATH}/TokenStreamHiddenTokenFilter.cpp
  ${ANTLR_LIB_SOURCE_PATH}/TokenStreamRewriteEngine.cpp
  ${ANTLR_LIB_SOURCE_PATH}/TokenStreamSelector.cpp
  ${ANTLR_LIB_SOURCE_PATH}/TreeParser.cpp
)

set_target_properties(${target}
    PROPERTIES 
  PUBLIC_HEADER "${ANTLR_PUBLIC_HEADERS}"
  OUTPUT_NAME "${ANTLR_LIB_NAME}"
)

##########################################################################
#
#   Compile & Link Options
#

target_compile_options(${target} PRIVATE $<IF:$<STREQUAL:${CMAKE_CXX_COMPILER_FRONTEND_VARIANT},MSVC>,-W3 -wd4996,-Wall>)
target_include_directories(${target} PUBLIC "${ANTLR_LIB_PATH}")

##########################################################################
#
#   Antlr2 as a CMake target
#   (to communicate the Antlr2 Usage Requirements to importers)
#

if(antlr2_POPULATED)
  if(NOT TARGET antlr2::antlr2)
    add_library(antlr2::antlr2 UNKNOWN IMPORTED GLOBAL)
    set_target_properties(antlr2::antlr2
        PROPERTIES
      SYSTEM OFF
      INTERFACE_INCLUDE_DIRECTORIES "${ANTLR_LIB_PATH}"
      IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${ANTLR_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}"
      IMPORTED_LINK_INTERFACE_LANGUAGES "C++"
    )
    # include(DebugTargetProperties OPTIONAL RESULT_VARIABLE DebugTargetProperties_FOUND)
    # if (DebugTargetProperties_FOUND)
    #   report_target_properties(${target})
    #   report_target_properties(antlr2::antlr2)
    # endif()
  endif()
endif()