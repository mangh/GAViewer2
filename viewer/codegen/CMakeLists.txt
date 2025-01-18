# Code generator for the GAViewer console
#

message(DEBUG "Entering the GAViewer/codegen subdirectory ...")

set(generator "gencode")	# as of CMake 3.31, "codegen" is a CMake reserved target name

add_executable(${generator}
  ${gavr_SOURCE_DIR}/src/console/codegen/codetemplate.cpp
  ${gavr_SOURCE_DIR}/src/console/codegen/main.cpp
)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
  # This is a WIN32 console application with the usual entry point "main" (not "WinMain")
  set_target_properties(${generator} PROPERTIES WIN32_EXECUTABLE OFF)
  target_compile_definitions(${generator} PRIVATE WIN32)
endif()

# back to the original executable name
set_target_properties(${generator} PROPERTIES OUTPUT_NAME "codegen")
set(codegen_exe "${CMAKE_CURRENT_BINARY_DIR}/codegen")

##########################################################################
#
#   codegen command:
#

add_custom_command(
  OUTPUT
    "${gavr_SOURCE_DIR}/src/console/consolegeneratedfunctions.cpp"
  COMMAND
    ${codegen_exe} "${gavr_SOURCE_DIR}/src/console/consolecodetemplates.txt" "${gavr_SOURCE_DIR}/src/console/consolegeneratedfunctions.cpp"
  DEPENDS
    "${gavr_SOURCE_DIR}/src/console/consolecodetemplates.txt"
    "${generator}"
  # WORKING_DIRECTORY
  #   "${CMAKE_CURRENT_BINARY_DIR}"
  COMMENT
    "${codegen_exe} ${gavr_SOURCE_DIR}/src/console/consolecodetemplates.txt ${gavr_SOURCE_DIR}/src/console/consolegeneratedfunctions.cpp"
  USES_TERMINAL
)

##########################################################################
#
#   A custom target that will trigger the (above) codegen command
#

add_custom_target(CONSOLEGENERATEDFUNCTIONS ALL DEPENDS "${gavr_SOURCE_DIR}/src/console/consolegeneratedfunctions.cpp")
