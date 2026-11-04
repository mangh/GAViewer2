# Interactive Book Figures for GAViewer
# See: https://geometricalgebra.org/figures.html
#

message(DEBUG "Entering the Interactive Book Figures subdirectory ...")

##########################################################################
#
#	  Download figures
#
#   Name: figures.zip
#   Size: 134012 bytes : 130 KiB
#   SHA256: 7d0f6f29fba1833ea6f4ccf0eb983cf5a0b1b2cec3c1f591f42f6352a0f54de1

Include(FetchContent)

FetchContent_Declare(
  figs
  # URL https://geometricalgebra.org/downloads/figures.zip
  URL "c:/Users/marek/source/repos/GA/exercises/figures.zip"
  URL_HASH SHA256=7d0f6f29fba1833ea6f4ccf0eb983cf5a0b1b2cec3c1f591f42f6352a0f54de1
)

FetchContent_MakeAvailable(figs)
FetchContent_GetProperties(figs)

# message(DEBUG "figs_POPULATED  = ${figs_POPULATED}")
# message(DEBUG "figs_SOURCE_DIR = ${figs_SOURCE_DIR}")
# message(DEBUG "figs_BINARY_DIR = ${figs_BINARY_DIR}")

# set(fig_SRC_PATH "${fig_SOURCE_DIR}/original")

##########################################################################
#
#	  Patching
#
#

set(figs_to_patch
  "${figs_SOURCE_DIR}/FIG_2_2.g"
)

# Patch file name and (working) path:
set(PATCH_FILE_NAME "figures.patch")
set(PATCH_FILE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${PATCH_FILE_NAME}")

# Paths to the SOURCE and TARGET folders (relative to the CMAKE_SOURCE_DIR folder)
# required by the following call to configure_file(), which creates the patch file:
string(REPLACE "${CMAKE_SOURCE_DIR}" "." PATCH_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
string(REPLACE "${CMAKE_SOURCE_DIR}" "." PATCH_TARGET_DIR "${figs_SOURCE_DIR}")

# Configure the patch file for the currently used CMAKE_BINARY_DIR folder
# (so that it works whether it is “${CMAKE_SOURCE_DIR}/build” or something else):
configure_file("${PATCH_FILE_NAME}.in" "${PATCH_FILE_NAME}" @ONLY)

add_custom_command(
  OUTPUT
    ${figs_to_patch}
  COMMAND
    # Original files are required to apply the patch (but are deleted after the patch is applied)
    ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/original" "${CMAKE_CURRENT_BINARY_DIR}"
  COMMAND
    ${GIT_EXECUTABLE} apply --unsafe-paths "${PATCH_FILE_PATH}"
  DEPENDS
    "${PATCH_FILE_PATH}"
  WORKING_DIRECTORY
    # NOTE: the command must be run in the CMAKE_SOURCE_DIR directory; otherwise it will not work!
    "${CMAKE_SOURCE_DIR}"
  COMMENT
    "The ${PATCH_FILE_PATH} file applied to the ${figs_SOURCE_DIR} folder."
  VERBATIM
  USES_TERMINAL
)

##########################################################################
#
#   Figures (patched) for GAViewer
#

set(figures_list
  ${figs_SOURCE_DIR}/conformal_paraboloid.geo
  ${figs_SOURCE_DIR}/factored.g
  ${figs_SOURCE_DIR}/FIG_10_2.g
  ${figs_SOURCE_DIR}/FIG_10_3.g
  ${figs_SOURCE_DIR}/FIG_10_4.g
  ${figs_SOURCE_DIR}/FIG_11_1.g
  ${figs_SOURCE_DIR}/FIG_11_10.g
  ${figs_SOURCE_DIR}/FIG_11_12.g
  ${figs_SOURCE_DIR}/FIG_11_2.g
  ${figs_SOURCE_DIR}/FIG_11_5.g
  ${figs_SOURCE_DIR}/FIG_11_6.g
  ${figs_SOURCE_DIR}/FIG_12_1.g
  ${figs_SOURCE_DIR}/FIG_12_2.g
  ${figs_SOURCE_DIR}/FIG_12_3.g
  ${figs_SOURCE_DIR}/FIG_12_4.g
  ${figs_SOURCE_DIR}/FIG_12_5.g
  ${figs_SOURCE_DIR}/FIG_13_2.g
  ${figs_SOURCE_DIR}/FIG_13_3.g
  ${figs_SOURCE_DIR}/FIG_13_4.g
  ${figs_SOURCE_DIR}/FIG_13_6.g
  ${figs_SOURCE_DIR}/FIG_13_7.g
  ${figs_SOURCE_DIR}/FIG_14_1.g
  ${figs_SOURCE_DIR}/FIG_14_10.g
  ${figs_SOURCE_DIR}/FIG_14_2.g
  ${figs_SOURCE_DIR}/FIG_14_3.g
  ${figs_SOURCE_DIR}/FIG_14_4.g
  ${figs_SOURCE_DIR}/FIG_14_6.g
  ${figs_SOURCE_DIR}/FIG_14_7.g
  ${figs_SOURCE_DIR}/FIG_14_9.g
  ${figs_SOURCE_DIR}/FIG_15_1.g
  ${figs_SOURCE_DIR}/FIG_15_13.g
  ${figs_SOURCE_DIR}/FIG_15_2.g
  ${figs_SOURCE_DIR}/FIG_15_3.g
  ${figs_SOURCE_DIR}/FIG_15_4.g
  ${figs_SOURCE_DIR}/FIG_15_5.g
  ${figs_SOURCE_DIR}/FIG_15_6.g
  ${figs_SOURCE_DIR}/FIG_15_7.g
  ${figs_SOURCE_DIR}/FIG_15_8.g
  ${figs_SOURCE_DIR}/FIG_15_9.g
  ${figs_SOURCE_DIR}/FIG_16_1.g
  ${figs_SOURCE_DIR}/FIG_16_10.g
  ${figs_SOURCE_DIR}/FIG_16_11.g
  ${figs_SOURCE_DIR}/FIG_16_12.g
  ${figs_SOURCE_DIR}/FIG_16_13.g
  ${figs_SOURCE_DIR}/FIG_16_2.g
  ${figs_SOURCE_DIR}/FIG_16_3.g
  ${figs_SOURCE_DIR}/FIG_16_6.g
  ${figs_SOURCE_DIR}/FIG_16_7.g
  ${figs_SOURCE_DIR}/FIG_16_8.g
  ${figs_SOURCE_DIR}/FIG_16_9.g
  ${figs_SOURCE_DIR}/FIG_1_1.g
  ${figs_SOURCE_DIR}/FIG_1_3.g
  ${figs_SOURCE_DIR}/FIG_1_4.g
  ${figs_SOURCE_DIR}/FIG_23_3.g
  ${figs_SOURCE_DIR}/FIG_2_1.g
  ${figs_SOURCE_DIR}/FIG_2_14.g
  ${figs_SOURCE_DIR}/FIG_2_2.g
  ${figs_SOURCE_DIR}/FIG_2_3.g
  ${figs_SOURCE_DIR}/FIG_2_4.g
  ${figs_SOURCE_DIR}/FIG_2_5.g
  ${figs_SOURCE_DIR}/FIG_2_6.g
  ${figs_SOURCE_DIR}/FIG_2_7.g
  ${figs_SOURCE_DIR}/FIG_2_8.g
  ${figs_SOURCE_DIR}/FIG_3_1.g
  ${figs_SOURCE_DIR}/FIG_3_2.g
  ${figs_SOURCE_DIR}/FIG_3_3.g
  ${figs_SOURCE_DIR}/FIG_3_4.g
  ${figs_SOURCE_DIR}/FIG_3_5.g
  ${figs_SOURCE_DIR}/FIG_3_6.g
  ${figs_SOURCE_DIR}/FIG_3_7.g
  ${figs_SOURCE_DIR}/FIG_3_8.g
  ${figs_SOURCE_DIR}/FIG_4_1.g
  ${figs_SOURCE_DIR}/FIG_4_2.g
  ${figs_SOURCE_DIR}/FIG_4_3.g
  ${figs_SOURCE_DIR}/FIG_5_1.g
  ${figs_SOURCE_DIR}/FIG_5_2.g
  ${figs_SOURCE_DIR}/FIG_5_3.g
  ${figs_SOURCE_DIR}/FIG_6_1.g
  ${figs_SOURCE_DIR}/FIG_6_2.g
  ${figs_SOURCE_DIR}/FIG_6_3.g
  ${figs_SOURCE_DIR}/FIG_6_4.g
  ${figs_SOURCE_DIR}/FIG_6_5.g
  ${figs_SOURCE_DIR}/FIG_7_1.g
  ${figs_SOURCE_DIR}/FIG_7_2.g
  ${figs_SOURCE_DIR}/FIG_7_3.g
  ${figs_SOURCE_DIR}/FIG_7_6.g
  ${figs_SOURCE_DIR}/FIG_7_7.g
  ${figs_SOURCE_DIR}/FIG_7_8.g
  ${figs_SOURCE_DIR}/FIG_8_1.g
  ${figs_SOURCE_DIR}/FIG_8_2.g
  ${figs_SOURCE_DIR}/FIG_8_3.g
  ${figs_SOURCE_DIR}/FIGS.g
  ${figs_SOURCE_DIR}/init.g
  ${figs_SOURCE_DIR}/Log.g
  ${figs_SOURCE_DIR}/orbiter.g
  ${figs_SOURCE_DIR}/orient.g
  ${figs_SOURCE_DIR}/pantilt.g
  ${figs_SOURCE_DIR}/unit.g
  ${figs_SOURCE_DIR}/vtrail.g
)

set(target "figures")

add_custom_target(${target} ALL DEPENDS ${figures_list})

##########################################################################
#
#   INSTALL
#

install(
  FILES ${figures_list}
  # TYPE DATA
  DESTINATION figures
  COMPONENT Figures
)

#   After compilation, the application can be installed using a command similar to the following:
#
#     cmake --install ~/GAViewer/build [--component Figures] --prefix /my/install/prefix
#
#   For example, on Windows, it could be:
#
#     cmake --install %USERPROFILE%\source\repos\GAViewer\build [--component Figures] --prefix %LOCALAPPDATA%\Programs\GAViewer
#
#   If you omit the '--component' parameter, CMake will install everything i.e.
#   the GAViewer executable, sample figures, but also FLTK (libraries and headers).
#