# Code generator for the GAViewer console
#

message(DEBUG "Entering the GAViewer/codegen subdirectory ...")

set(generator "gencode")	# as of CMake 3.31, "codegen" is a CMake reserved target name
set(generator_output_name "codegen")

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
set_target_properties(${generator} PROPERTIES OUTPUT_NAME "${generator_output_name}")

##########################################################################
#
#   codegen command
#

set(console_src_dir "${gavr_SOURCE_DIR}/src/console")
set(codegen_cmd "${CMAKE_CURRENT_BINARY_DIR}/${generator_output_name}")

add_custom_command(
  OUTPUT
    "${console_src_dir}/consolegeneratedfunctions.cpp"
  COMMAND
    ${codegen_cmd} "${console_src_dir}/consolecodetemplates.txt" "${console_src_dir}/consolegeneratedfunctions.cpp"
  DEPENDS
    "${console_src_dir}/consolecodetemplates.txt"
    "${generator}"
  # WORKING_DIRECTORY
  #   "${console_src_dir}"
  COMMENT
    "${codegen_cmd} ${console_src_dir}/consolecodetemplates.txt ${console_src_dir}/consolegeneratedfunctions.cpp"
  USES_TERMINAL
)

##########################################################################
#
#   A custom target that will trigger the (above) codegen command
#

add_custom_target(CONSOLEGENERATEDFUNCTIONS ALL DEPENDS "${gavr_SOURCE_DIR}/src/console/consolegeneratedfunctions.cpp")
