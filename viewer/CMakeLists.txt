# GAViewer (multi-purpose program for geometric algebra computations and visualization)
#

message(DEBUG "Entering the GAViewer subdirectory ...")

Include(FetchContent)

# Name: GAViewer-0.86.tar.gz
# Size: 1274417 bytes : 1244 KiB
# SHA256: 2bdfbb809c45453edc2fbdcd8b3d0b5937c688ac4795ceb31da4c67d657406cd

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

##########################################################################
#
#	  Patching
#
#	  src/mainwindow.h
#     - missing #include <windows.h> directive to #define LONG typedef
#
#	  src/typeset/fontyacc.c
#     - fontyacc.c:63:8: error: type specifier missing, defaults to 'int'; ISO C99 and later do not support implicit int [-Wimplicit-int]
#     - fontyacc.c:67:8: error: type specifier missing, defaults to 'int'; ISO C99 and later do not support implicit int [-Wimplicit-int]
#
#	  src/typeset/typesetbox.cpp
#     - typesetbox.cpp:195:17: error: non-constant-expression cannot be narrowed from type 'int' to 'char' in initializer list [-Wc++11-narrowing]
#

# Save the original (unpatched) file and put the patched file instead
if (WIN32)
  set(_deprecated "src/mainwindow.h")
  file(STRINGS "${gavr_SOURCE_DIR}/${_deprecated}" _patch_applied REGEX "^#include <windows.h>$")
  if (_patch_applied)
    message(DEBUG "The ${gavr_SOURCE_DIR}/${_deprecated} file has already been patched.")
  else()
    file(COPY_FILE "${gavr_SOURCE_DIR}/${_deprecated}" "${CMAKE_CURRENT_SOURCE_DIR}/${_deprecated}.original")
    file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${_deprecated}.patched" "${gavr_SOURCE_DIR}/${_deprecated}")
    message(DEBUG "The ${gavr_SOURCE_DIR}/${_deprecated} file has been patched.")
  endif()
endif()

set(_deprecated "src/typeset/fontyacc.c")
file(STRINGS "${gavr_SOURCE_DIR}/${_deprecated}" _patch_applied REGEX "^void static yyerror\\(const char \\*msg\\) \\{$")
if (_patch_applied)
  message(DEBUG "The ${gavr_SOURCE_DIR}/${_deprecated} file has already been patched.")
else()
  file(COPY_FILE "${gavr_SOURCE_DIR}/${_deprecated}" "${CMAKE_CURRENT_SOURCE_DIR}/${_deprecated}.original")
  file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${_deprecated}.patched" "${gavr_SOURCE_DIR}/${_deprecated}")
  message(DEBUG "The ${gavr_SOURCE_DIR}/${_deprecated} file has been patched.")
endif()

set(_deprecated "src/typeset/typesetbox.cpp")
file(STRINGS "${gavr_SOURCE_DIR}/${_deprecated}" _patch_applied REGEX "^[ \t]+char str\\[2\\] = \\{\\(char\\)c, \\(char\\)0\\};$")
if (_patch_applied)
  message(DEBUG "The ${gavr_SOURCE_DIR}/${_deprecated} file has already been patched.")
else()
  file(COPY_FILE "${gavr_SOURCE_DIR}/${_deprecated}" "${CMAKE_CURRENT_SOURCE_DIR}/${_deprecated}.original")
  file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${_deprecated}.patched" "${gavr_SOURCE_DIR}/${_deprecated}")
  message(DEBUG "The ${gavr_SOURCE_DIR}/${_deprecated} file has been patched.")
endif()


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
  WIN32
  ${_source_files}
)

##########################################################################
#
#   COMPILE DEFINITIONS & OPTIONS
#

target_compile_features(${target} PRIVATE cxx_std_14)
target_compile_options(${target} PRIVATE $<IF:$<STREQUAL:${CMAKE_CXX_COMPILER_FRONTEND_VARIANT},MSVC>,/W3,-Wall>) # but not -Wextra

target_compile_definitions(${target} PRIVATE WIN32)
target_compile_definitions(${target} PRIVATE _CONSOLE)
target_compile_definitions(${target} PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_definitions(${target} PRIVATE WIN32_NO_CONSOLE)
target_compile_definitions(${target} PRIVATE _MBCS)

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

