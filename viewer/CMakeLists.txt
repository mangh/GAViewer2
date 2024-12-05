# GAViewer (multi-purpose program for geometric algebra computations and visualization)
#

message(DEBUG "Entering the GAViewer subdirectory ...")

##########################################################################
#
#   Directory compile options
#
#   When compiling GAViewer source code (which is years old), 
#   many deprecation warnings appear. It's worth turning them off to get
#   a clearer picture of the compilation and spot more serious issues (if any).
#

if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC")

  add_compile_options(
    -D_CRT_SECURE_NO_WARNINGS
    -wd4996
  )

elseif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "GNU")

  add_compile_options(
    -Wno-deprecated
    -Wno-deprecated-non-prototype # GAViewer
  )

endif()

##########################################################################
#
#	  Downloading source files
#
#   Name: GAViewer-0.86.tar.gz
#   Size: 1274417 bytes : 1244 KiB
#   SHA256: 2bdfbb809c45453edc2fbdcd8b3d0b5937c688ac4795ceb31da4c67d657406cd
#

Include(FetchContent)

FetchContent_Declare(
  gavr
  URL https://geometricalgebra.org/downloads/GAViewer-0.86.tar.gz
  URL_HASH SHA256=2bdfbb809c45453edc2fbdcd8b3d0b5937c688ac4795ceb31da4c67d657406cd
)

FetchContent_MakeAvailable(gavr)

FetchContent_GetProperties(gavr)

message(DEBUG "gavr_POPULATED = ${gavr_POPULATED}")
message(DEBUG "gavr_SOURCE_DIR = ${gavr_SOURCE_DIR}")
message(DEBUG "gavr_BINARY_DIR = ${gavr_BINARY_DIR}")

# ##########################################################################
# #
# #	  codegen
# #
# #
# 
# set(CODEGEN_DIR "${gavr_SOURCE_DIR}/src/console/codegen")
# add_subdirectory("codegen")

##########################################################################
#
#	  Patching
#
#

include(Patch)

if (WIN32)
  patch(
    "${CMAKE_CURRENT_SOURCE_DIR}" # patch dir (containing the patched file)
    "${gavr_SOURCE_DIR}"          # source dir (containing file to patch)
    "src/mainwindow.h"            # [relative path/]name of the file to patch
    "^#include <windows\.h>$"      # regex (for a string that must be in the patched file)
  )
  
  patch(
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${gavr_SOURCE_DIR}"
    "src/gaviewer.rc"
    "^#include \"windows\.h\"$"
  )
endif()

patch(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${gavr_SOURCE_DIR}"
  "src/typeset/fontparse.cpp"
  "^[ \t]+printf\\(\"fpSetChild\\(\\): invalid input %p %p %d\\\\n\", parent, child, idx\\)\;$"
)

patch(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${gavr_SOURCE_DIR}"
  "src/typeset/fontyacc.c"
  "^void static yyerror\\(const char \\*msg\\) \\{$"
)

patch(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${gavr_SOURCE_DIR}"
  "src/typeset/liststack.h"
  "^[ \t]+printf\\(\"tsItemList::ensureMax\\(\\): memory allocation failure for '%zu' bytes\\\\n\", sizeof\\(type\\)\\)\;$"
)

patch(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${gavr_SOURCE_DIR}"
  "src/typeset/typeset.cpp"
  "^[ \t]+printf\\(\"typesetState::addLine\\(\\): memory allocation failure for '%zu' bytes\\\\n\", sizeof\\(double\\) \\* 2 \\* nbVertices\\)\;$"
)

patch(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${gavr_SOURCE_DIR}"
  "src/typeset/typesetbox.cpp"
  "^[ \t]+char str\\[2\\] = \\{\\(char\\)c, \\(char\\)0\\}\;$"
)

patch(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${gavr_SOURCE_DIR}"
  "src/c3ga/c3ga.cpp"
  "^[ \t]+printf\\(\"c3ga_ben::addName\\(\\): memory allocation failure for '%zu' bytes\\\\n\", sizeof\\(c3gai_be\\*\\) \\* \\(m_nbBen \\+ 1\\)\\)\;$"
)

patch(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${gavr_SOURCE_DIR}"
  "src/c5ga/c5ga.cpp"
  "^[ \t]+printf\\(\"c5ga_ben::addName\\(\\): memory allocation failure for '%zu' bytes\\\\n\", sizeof\\(c5gai_be\\*\\) \\* \\(m_nbBen \\+ 1\\)\\)\;$"
)

patch(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${gavr_SOURCE_DIR}"
  "src/e3ga/e3ga.cpp"
  "^[ \t]+printf\\(\"e3ga_ben::addName\\(\\): memory allocation failure for '%zu' bytes\\\\n\", sizeof\\(e3gai_be\\*\\) \\* \\(m_nbBen \\+ 1\\)\\)\;$"
)

patch(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${gavr_SOURCE_DIR}"
  "src/i2ga/i2ga.cpp"
  "^[ \t]+printf\\(\"i2ga_ben::addName\\(\\): memory allocation failure for '%zu' bytes\\\\n\", sizeof\\(i2gai_be\\*\\) \\* \\(m_nbBen \\+ 1\\)\\)\;$"
)

patch(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${gavr_SOURCE_DIR}"
  "src/p3ga/p3ga.cpp"
  "^[ \t]+printf\\(\"p3ga_ben::addName\\(\\): memory allocation failure for '%zu' bytes\\\\n\", sizeof\\(p3gai_be\\*\\) \\* \\(m_nbBen \\+ 1\\)\\)\;$"
)

##########################################################################
#
#	GAViewer executable target to build
#

set(target "GAViewer")

set(_source_files
    "${gavr_SOURCE_DIR}/src/c3gaobject.cpp"
    "${gavr_SOURCE_DIR}/src/c3ga/c3ga.cpp"
    "${gavr_SOURCE_DIR}/src/c3ga/c3ga_optc.cpp"
    "${gavr_SOURCE_DIR}/src/c5gaobject.cpp"
    "${gavr_SOURCE_DIR}/src/c5ga/c5ga.cpp"
    "${gavr_SOURCE_DIR}/src/c5ga/c5ga_optc.cpp"
    "${gavr_SOURCE_DIR}/src/conformalobject.cpp"
    "${gavr_SOURCE_DIR}/src/console/console.cpp"
    "${gavr_SOURCE_DIR}/src/console/consoleautocolor.cpp"
    "${gavr_SOURCE_DIR}/src/console/consolecontrol.cpp"
    "${gavr_SOURCE_DIR}/src/console/consoledynstmt.cpp"
    "${gavr_SOURCE_DIR}/src/console/consolefunctions.cpp"
    "${gavr_SOURCE_DIR}/src/console/consolegafile.cpp"
    "${gavr_SOURCE_DIR}/src/console/consolegeneratedfunctions.cpp"
    "${gavr_SOURCE_DIR}/src/console/ConsoleLexer.cpp"
    "${gavr_SOURCE_DIR}/src/console/consoleoperators.cpp"
    "${gavr_SOURCE_DIR}/src/console/ConsoleParser.cpp"
    "${gavr_SOURCE_DIR}/src/console/consolescope.cpp"
    "${gavr_SOURCE_DIR}/src/console/consolesourcefileread.cpp"
    "${gavr_SOURCE_DIR}/src/console/consolestatement.cpp"
    "${gavr_SOURCE_DIR}/src/console/consoletest.cpp"
    "${gavr_SOURCE_DIR}/src/console/consolevariable.cpp"
    "${gavr_SOURCE_DIR}/src/console/consolewindow.cpp"
    "${gavr_SOURCE_DIR}/src/console/fl_console.cpp"
    "${gavr_SOURCE_DIR}/src/draw.cpp"
    "${gavr_SOURCE_DIR}/src/dynamicstatementwindow.cpp"
    "${gavr_SOURCE_DIR}/src/e3gaobject.cpp"
    "${gavr_SOURCE_DIR}/src/e3ga/e3ga.cpp"
    "${gavr_SOURCE_DIR}/src/e3ga/e3ga_optc.cpp"
    "${gavr_SOURCE_DIR}/src/gafile.cpp"
    "${gavr_SOURCE_DIR}/src/geosphere.cpp"
    "${gavr_SOURCE_DIR}/src/glwindow.cpp"
    "${gavr_SOURCE_DIR}/src/i2gaobject.cpp"
    "${gavr_SOURCE_DIR}/src/i2ga/i2ga.cpp"
    "${gavr_SOURCE_DIR}/src/i2ga/i2ga_optc2.cpp"
    "${gavr_SOURCE_DIR}/src/main.cpp"
    "${gavr_SOURCE_DIR}/src/mainwindow.cpp"
    "${gavr_SOURCE_DIR}/src/meshobject.cpp"
    "${gavr_SOURCE_DIR}/src/mt19937-2.cpp"
    "${gavr_SOURCE_DIR}/src/mvint.cpp"
    "${gavr_SOURCE_DIR}/src/mvintc3ga.cpp"
    "${gavr_SOURCE_DIR}/src/mvintc5ga.cpp"
    "${gavr_SOURCE_DIR}/src/mvinte3ga.cpp"
    "${gavr_SOURCE_DIR}/src/mvinti2ga.cpp"
    "${gavr_SOURCE_DIR}/src/mvintp3ga.cpp"
    "${gavr_SOURCE_DIR}/src/netclient.cpp"
    "${gavr_SOURCE_DIR}/src/netserver.cpp"
    "${gavr_SOURCE_DIR}/src/object.cpp"
    "${gavr_SOURCE_DIR}/src/osdep.cpp"
    "${gavr_SOURCE_DIR}/src/p3gaobject.cpp"
    "${gavr_SOURCE_DIR}/src/p3ga/p3ga.cpp"
    "${gavr_SOURCE_DIR}/src/p3ga/p3ga_optc.cpp"
    "${gavr_SOURCE_DIR}/src/pngwrapper.cpp"
    "${gavr_SOURCE_DIR}/src/polygonobject.cpp"
    "${gavr_SOURCE_DIR}/src/screenshot.cpp"
    "${gavr_SOURCE_DIR}/src/screenshotdialog.cpp"
    "${gavr_SOURCE_DIR}/src/selectwindow.cpp"
    "${gavr_SOURCE_DIR}/src/softpic.cpp"
    "${gavr_SOURCE_DIR}/src/state.cpp"
    "${gavr_SOURCE_DIR}/src/textobject.cpp"
    "${gavr_SOURCE_DIR}/src/tubedraw.cpp"
    "${gavr_SOURCE_DIR}/src/typeset/fontdata.cpp"
    "${gavr_SOURCE_DIR}/src/typeset/fontlex.c"
    "${gavr_SOURCE_DIR}/src/typeset/fontparse.cpp"
    "${gavr_SOURCE_DIR}/src/typeset/fontyacc.c"
    "${gavr_SOURCE_DIR}/src/typeset/openglfont.cpp"
#      <ObjectFileName Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">$(IntDir)%(Filename)1.obj</ObjectFileName>
#      <XMLDocumentationFileName Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">$(IntDir)%(Filename)1.xdc</XMLDocumentationFileName>
#      <ObjectFileName Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">$(IntDir)%(Filename)1.obj</ObjectFileName>
#      <XMLDocumentationFileName Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">$(IntDir)%(Filename)1.xdc</XMLDocumentationFileName>
#      <ObjectFileName Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">$(IntDir)%(Filename)1.obj</ObjectFileName>
#      <XMLDocumentationFileName Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">$(IntDir)%(Filename)1.xdc</XMLDocumentationFileName>
#      <ObjectFileName Condition="'$(Configuration)|$(Platform)'=='Release|x64'">$(IntDir)%(Filename)1.obj</ObjectFileName>
#      <XMLDocumentationFileName Condition="'$(Configuration)|$(Platform)'=='Release|x64'">$(IntDir)%(Filename)1.xdc</XMLDocumentationFileName>
    "${gavr_SOURCE_DIR}/src/typeset/typeset.cpp"
    "${gavr_SOURCE_DIR}/src/typeset/typesetbox.cpp"
    "${gavr_SOURCE_DIR}/src/ui.cpp"
    "${gavr_SOURCE_DIR}/src/util.cpp"
    "${gavr_SOURCE_DIR}/src/writestate.cpp"
)

# Add executable target to the project using the specified source files:
add_executable(
  ${target}
  ${_source_files}
)
# On Windows, it must be an executable with a WinMain entry point:
set_target_properties(${target} PROPERTIES WIN32_EXECUTABLE $<$<BOOL:WIN32>:ON>)
# Add resources (icon) on Windows
if (WIN32)
  target_sources(${target} PRIVATE "${gavr_SOURCE_DIR}/src/gaviewer.rc")
endif()


##########################################################################
#
#   COMPILE DEFINITIONS & OPTIONS
#

target_compile_features(${target} PRIVATE cxx_std_14)
# target_compile_options(${target} PRIVATE $<IF:$<STREQUAL:${CMAKE_CXX_COMPILER_FRONTEND_VARIANT},MSVC>,/W3,-Wall>) # but not -Wextra
target_compile_options(${target} PRIVATE $<IF:$<STREQUAL:${CMAKE_CXX_COMPILER_FRONTEND_VARIANT},MSVC>,/W3,-Wno-parentheses>)

if (WIN32)
  target_compile_definitions(
      ${target}
    PRIVATE WIN32             # Application being built for Windows
    PRIVATE WIN32_NO_CONSOLE  # without a console
    PRIVATE _MBCS             # for Multibyte Character Set
    # PRIVATE _CRT_SECURE_NO_WARNINGS # already defined in the top level CMakeLists.txt
  )
endif()

##########################################################################
#
#   ADDRESS SANITIZER
#

if(USE_ASAN)
  # The following is required to avoid: lld-link: error: /INFERASANLIBS is not allowed in .drectve
  target_compile_definitions(${target} PRIVATE _DISABLE_STRING_ANNOTATION _DISABLE_VECTOR_ANNOTATION)
  target_compile_options(${target} PRIVATE -fsanitize=address)
  target_link_options(${target} PRIVATE -fsanitize=address)
endif()

##########################################################################
#
#   LINK OPTIONS & LIBRARIES
#

# IPO/LTO optimization makes the application just as efficient
# whether it uses units of measure or simple numbers.
if(LTO_OPTIMIZATION_SUPPORTED)
  set_property(TARGET ${target} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(WARNING "IPO/LTO is not supported: ${LTO_OPTIMIZATION_ERROR}")
endif()

target_link_libraries(${target}
    PRIVATE
  fltk::fltk
  fltk::gl
  fltk::png
  antlr2::antlr2
)

set_target_properties(${target}
    PROPERTIES
  LINK_WHAT_YOU_USE ON
)

##########################################################################
#
#   INSTALL
#
#   Use the following command (in the project's root directory) to
#   run the application:
#
#       ./build/${testApp} /path/to/input_data.txt
#
#   For example:
#
#       ./build/${testApp} ./CALINE3.EXP
#

# set(testApp "${target}v${APP_VER_CFG}")
# set_target_properties(${target} PROPERTIES OUTPUT_NAME "${testApp}")

install(TARGETS ${target}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

