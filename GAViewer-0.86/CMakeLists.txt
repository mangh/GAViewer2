# GAViewer (multi-purpose program for geometric algebra computations and visualization)
#

message(DEBUG "Entering the GAViewer subdirectory ...")

# GAViewer console uses code generator app
add_subdirectory("codegen")

set(target "GAViewer")

set(_source_files
    "${CMAKE_CURRENT_SOURCE_DIR}/src/c3gaobject.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/c3ga/c3ga.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/c3ga/c3ga_optc.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/c5gaobject.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/c5ga/c5ga.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/c5ga/c5ga_optc.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/conformalobject.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/console/console.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/console/consoleautocolor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/console/consolecontrol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/console/consoledynstmt.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/console/consolefunctions.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/console/consolegafile.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/console/consolegeneratedfunctions.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/console/ConsoleLexer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/console/consoleoperators.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/console/ConsoleParser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/console/consolescope.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/console/consolesourcefileread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/console/consolestatement.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/console/consoletest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/console/consolevariable.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/console/consolewindow.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/console/fl_console.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/draw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dynamicstatementwindow.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/e3gaobject.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/e3ga/e3ga.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/e3ga/e3ga_optc.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gafile.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/geosphere.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glwindow.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/i2gaobject.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/i2ga/i2ga.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/i2ga/i2ga_optc2.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mainwindow.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/meshobject.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mt19937-2.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mvint.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mvintc3ga.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mvintc5ga.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mvinte3ga.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mvinti2ga.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mvintp3ga.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/netclient.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/netserver.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/object.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/osdep.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/p3gaobject.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/p3ga/p3ga.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/p3ga/p3ga_optc.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/pngwrapper.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/polygonobject.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/screenshot.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/screenshotdialog.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/selectwindow.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/softpic.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/state.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/textobject.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tubedraw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/typeset/fontdata.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/typeset/fontlex.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/typeset/fontparse.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/typeset/fontyacc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/typeset/openglfont.cpp"
#      <ObjectFileName Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">$(IntDir)%(Filename)1.obj</ObjectFileName>
#      <XMLDocumentationFileName Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">$(IntDir)%(Filename)1.xdc</XMLDocumentationFileName>
#      <ObjectFileName Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">$(IntDir)%(Filename)1.obj</ObjectFileName>
#      <XMLDocumentationFileName Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">$(IntDir)%(Filename)1.xdc</XMLDocumentationFileName>
#      <ObjectFileName Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">$(IntDir)%(Filename)1.obj</ObjectFileName>
#      <XMLDocumentationFileName Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">$(IntDir)%(Filename)1.xdc</XMLDocumentationFileName>
#      <ObjectFileName Condition="'$(Configuration)|$(Platform)'=='Release|x64'">$(IntDir)%(Filename)1.obj</ObjectFileName>
#      <XMLDocumentationFileName Condition="'$(Configuration)|$(Platform)'=='Release|x64'">$(IntDir)%(Filename)1.xdc</XMLDocumentationFileName>
    "${CMAKE_CURRENT_SOURCE_DIR}/src/typeset/typeset.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/typeset/typesetbox.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/util.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/writestate.cpp"
)

# Add executable target to the project using the specified source files:
add_executable(
  ${target}
  ${_source_files}
)

##########################################################################
#
#   COMPILE DEFINITIONS & OPTIONS
#

target_compile_features(${target} PRIVATE cxx_std_14)
target_compile_options(${target} PRIVATE $<IF:$<STREQUAL:${CMAKE_CXX_COMPILER_FRONTEND_VARIANT},MSVC>,/W3,-Wall -Wextra>)

target_compile_definitions(${target} PRIVATE _CONSOLE)
target_compile_definitions(${target} PRIVATE _CRT_SECURE_NO_WARNINGS)
#target_compile_definitions(${target} PRIVATE WIN32_NO_CONSOLE)
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

