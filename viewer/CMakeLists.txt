# GAViewer (multi-purpose program for geometric algebra computations and visualization)
#

message(DEBUG "Entering the GAViewer subdirectory ...")

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

# message(DEBUG "gavr_POPULATED = ${gavr_POPULATED}")
# message(DEBUG "gavr_SOURCE_DIR = ${gavr_SOURCE_DIR}")
# message(DEBUG "gavr_BINARY_DIR = ${gavr_BINARY_DIR}")

set(gavr_SRC_PATH "${gavr_SOURCE_DIR}/src")

# ##########################################################################
# #
# #	  codegen - useless
# #
# #
# 
# add_subdirectory("codegen")

##########################################################################
#
#	  Patching
#
#

# Source and target file directories (required for the configure_file() call below):
set(PATCH_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(PATCH_TARGET_DIR "${gavr_SOURCE_DIR}")

# Patch file name and (working) path:
set(PATCH_FILE_NAME "viewer.patch")
set(PATCH_FILE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${PATCH_FILE_NAME}")

# Configure the patch file for the CMAKE_BINARY_DIR (working) folder
# that is currently in use (“build” or whatever):
configure_file("${PATCH_FILE_NAME}.in" "${PATCH_FILE_NAME}" @ONLY)

add_custom_command(
  OUTPUT
    "${gavr_SRC_PATH}/gaviewer.rc"
    "${gavr_SRC_PATH}/mainwindow.h"
    "${gavr_SRC_PATH}/osdep.h"
    "${gavr_SRC_PATH}/state.h"
    "${gavr_SRC_PATH}/ui.cpp"
    "${gavr_SRC_PATH}/c3ga/c3ga.cpp"
    "${gavr_SRC_PATH}/c5ga/c5ga.cpp"
    "${gavr_SRC_PATH}/e3ga/e3ga.cpp"
    "${gavr_SRC_PATH}/i2ga/i2ga.cpp"
    "${gavr_SRC_PATH}/p3ga/p3ga.cpp"
    "${gavr_SRC_PATH}/typeset/fontparse.cpp"
    "${gavr_SRC_PATH}/typeset/fontyacc.c"
    "${gavr_SRC_PATH}/typeset/liststack.h"
    "${gavr_SRC_PATH}/typeset/typeset.cpp"
    "${gavr_SRC_PATH}/typeset/typesetbox.cpp"
  COMMAND
    # Original files are required to apply the patch (but are deleted after the patch is applied)
    cmake -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_BINARY_DIR}/src"
  COMMAND
    ${GIT_EXECUTABLE} apply --unsafe-paths "${PATCH_FILE_PATH}"
  DEPENDS
    "${PATCH_FILE_PATH}"
  WORKING_DIRECTORY
    # NOTE: the command must be run in the CMAKE_SOURCE_DIR directory; otherwise it will not work!
    "${CMAKE_SOURCE_DIR}"
  COMMENT
    "Patch file ${PATCH_FILE_PATH} applied to the ${gavr_SRC_PATH} folder."
  USES_TERMINAL
)

##########################################################################
#
#	GAViewer executable target to build
#

set(target "GAViewer")

set(_source_files
    "${gavr_SRC_PATH}/c3gaobject.cpp"
    "${gavr_SRC_PATH}/c3ga/c3ga.cpp"
    "${gavr_SRC_PATH}/c3ga/c3ga_optc.cpp"
    "${gavr_SRC_PATH}/c5gaobject.cpp"
    "${gavr_SRC_PATH}/c5ga/c5ga.cpp"
    "${gavr_SRC_PATH}/c5ga/c5ga_optc.cpp"
    "${gavr_SRC_PATH}/conformalobject.cpp"
    "${gavr_SRC_PATH}/console/console.cpp"
    "${gavr_SRC_PATH}/console/consoleautocolor.cpp"
    "${gavr_SRC_PATH}/console/consolecontrol.cpp"
    "${gavr_SRC_PATH}/console/consoledynstmt.cpp"
    "${gavr_SRC_PATH}/console/consolefunctions.cpp"
    "${gavr_SRC_PATH}/console/consolegafile.cpp"
    "${gavr_SRC_PATH}/console/consolegeneratedfunctions.cpp"
    "${gavr_SRC_PATH}/console/ConsoleLexer.cpp"
    "${gavr_SRC_PATH}/console/consoleoperators.cpp"
    "${gavr_SRC_PATH}/console/ConsoleParser.cpp"
    "${gavr_SRC_PATH}/console/consolescope.cpp"
    "${gavr_SRC_PATH}/console/consolesourcefileread.cpp"
    "${gavr_SRC_PATH}/console/consolestatement.cpp"
    "${gavr_SRC_PATH}/console/consoletest.cpp"
    "${gavr_SRC_PATH}/console/consolevariable.cpp"
    "${gavr_SRC_PATH}/console/consolewindow.cpp"
    "${gavr_SRC_PATH}/console/fl_console.cpp"
    "${gavr_SRC_PATH}/draw.cpp"
    "${gavr_SRC_PATH}/dynamicstatementwindow.cpp"
    "${gavr_SRC_PATH}/e3gaobject.cpp"
    "${gavr_SRC_PATH}/e3ga/e3ga.cpp"
    "${gavr_SRC_PATH}/e3ga/e3ga_optc.cpp"
    "${gavr_SRC_PATH}/gafile.cpp"
    "${gavr_SRC_PATH}/geosphere.cpp"
    "${gavr_SRC_PATH}/glwindow.cpp"
    "${gavr_SRC_PATH}/i2gaobject.cpp"
    "${gavr_SRC_PATH}/i2ga/i2ga.cpp"
    "${gavr_SRC_PATH}/i2ga/i2ga_optc2.cpp"
    "${gavr_SRC_PATH}/main.cpp"
    "${gavr_SRC_PATH}/mainwindow.cpp"
    "${gavr_SRC_PATH}/meshobject.cpp"
    "${gavr_SRC_PATH}/mt19937-2.cpp"
    "${gavr_SRC_PATH}/mvint.cpp"
    "${gavr_SRC_PATH}/mvintc3ga.cpp"
    "${gavr_SRC_PATH}/mvintc5ga.cpp"
    "${gavr_SRC_PATH}/mvinte3ga.cpp"
    "${gavr_SRC_PATH}/mvinti2ga.cpp"
    "${gavr_SRC_PATH}/mvintp3ga.cpp"
    "${gavr_SRC_PATH}/netclient.cpp"
    "${gavr_SRC_PATH}/netserver.cpp"
    "${gavr_SRC_PATH}/object.cpp"
    "${gavr_SRC_PATH}/osdep.cpp"
    "${gavr_SRC_PATH}/p3gaobject.cpp"
    "${gavr_SRC_PATH}/p3ga/p3ga.cpp"
    "${gavr_SRC_PATH}/p3ga/p3ga_optc.cpp"
    "${gavr_SRC_PATH}/pngwrapper.cpp"
    "${gavr_SRC_PATH}/polygonobject.cpp"
    "${gavr_SRC_PATH}/screenshot.cpp"
    "${gavr_SRC_PATH}/screenshotdialog.cpp"
    "${gavr_SRC_PATH}/selectwindow.cpp"
    "${gavr_SRC_PATH}/softpic.cpp"
    "${gavr_SRC_PATH}/state.cpp"
    "${gavr_SRC_PATH}/textobject.cpp"
    "${gavr_SRC_PATH}/tubedraw.cpp"
    "${gavr_SRC_PATH}/typeset/fontdata.cpp"
    "${gavr_SRC_PATH}/typeset/fontlex.c"
    "${gavr_SRC_PATH}/typeset/fontparse.cpp"
    "${gavr_SRC_PATH}/typeset/fontyacc.c"
    "${gavr_SRC_PATH}/typeset/openglfont.cpp"
    "${gavr_SRC_PATH}/typeset/typeset.cpp"
    "${gavr_SRC_PATH}/typeset/typesetbox.cpp"
    "${gavr_SRC_PATH}/ui.cpp"
    "${gavr_SRC_PATH}/util.cpp"
    "${gavr_SRC_PATH}/writestate.cpp"
)

# Add executable target to the project using the specified source files:
add_executable(
  ${target}
  ${_source_files}
)

# Add resources (icon)
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
  target_sources(${target} PRIVATE "${gavr_SRC_PATH}/gaviewer.rc")
endif()

##########################################################################
#
#   COMPILE DEFINITIONS & OPTIONS
#

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
  
  target_compile_definitions(${target}
    PRIVATE WIN32             # application being built for Windows
    PRIVATE WIN32_NO_CONSOLE  # without a console
    PRIVATE _MBCS             # for Multibyte Character Set
  )

  set_target_properties(${target} PROPERTIES
    WIN32_EXECUTABLE ON       # the executable must have a WinMain entry point
  )

endif()

# When compiling GAViewer source code (which is years old), many
# deprecation warnings appear. It's worth turning them off to
# get a clearer picture of the compilation and spot more serious
# issues (if any).

if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC")

  target_compile_definitions(${target}
    # deprecation warnings for the older, less secure functions
    _CRT_SECURE_NO_WARNINGS   
  )
  target_compile_options(${target}
    -W3       # production quality warnings
    -wd4996   # deprecated function, class member, variable, or typedef
  )

elseif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "GNU")

  target_compile_options(${target}
    PRIVATE
    -Wall
    -Wno-deprecated
    -Wno-parentheses
    -Wno-unused-variable
  )
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${target}
      PRIVATE
      # function declaration without a prototype is deprecated in all versions of C
      -Wno-deprecated-non-prototype
    )
  endif()

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
  PRIVATE fltk::png
  PRIVATE fltk::gl
  PRIVATE fltk::fltk
  PRIVATE antlr2::antlr2
)

target_include_directories(${target}
  PRIVATE $<TARGET_PROPERTY:fltk::fltk,INTERFACE_INCLUDE_DIRECTORIES>)

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

# include(DebugTargetProperties OPTIONAL RESULT_VARIABLE DebugTargetProperties_FOUND)
# if (DebugTargetProperties_FOUND)
#   report_target_properties(${target})
# endif()
