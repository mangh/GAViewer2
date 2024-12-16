# Code generator for the GAViewer console
#

message(DEBUG "Entering the GAViewer codegen subdirectory ...")

set(generator "gencode")	# as of CMake 3.31, "codegen" is a CMake reserved target name

add_executable(${generator}
  ${gavr_SOURCE_DIR}/src/console/codegen/codetemplate.cpp
  ${gavr_SOURCE_DIR}/src/console/codegen/main.cpp
)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
  # This is a WIN32 console application with the usual entry point �main� (not �WinMain�)
  set_target_properties(${generator} PROPERTIES WIN32_EXECUTABLE OFF)
  target_compile_definitions(${generator} PRIVATE WIN32)
endif()
set_target_properties(${generator} PROPERTIES OUTPUT_NAME "codegen")	# back to the original executable name

##########################################################################
#
#   codegen command:
#
#   codegen ${gavr_SOURCE_DIR}/src/console/consolecodetemplates.txt ${gavr_SOURCE_DIR}/src/console/consolegeneratedfunctions.cpp
#
#

add_custom_command(
  OUTPUT
    "${gavr_SOURCE_DIR}/src/console/consolegeneratedfunctions.cpp"
  COMMAND cmake -E echo
  COMMAND cmake -E chdir "${CMAKE_CURRENT_BINARY_DIR}" codegen "${gavr_SOURCE_DIR}/src/console/consolecodetemplates.txt" "${gavr_SOURCE_DIR}/src/console/consolegeneratedfunctions.cpp"
  COMMAND cmake -E echo
  DEPENDS
    "${gavr_SOURCE_DIR}/src/console/consolecodetemplates.txt"
    "${generator}"
  COMMENT "${CMAKE_CURRENT_BINARY_DIR}/codegen ${gavr_SOURCE_DIR}/src/console/consolecodetemplates.txt ${gavr_SOURCE_DIR}/src/console/consolegeneratedfunctions.cpp"
  USES_TERMINAL
)

##########################################################################
#
#   consolegeneratedfunctions target
#
#   Custom target that triggers the REBUILD COMMAND
#

add_custom_target(CONSOLEGENERATEDFUNCTIONS DEPENDS "${gavr_SOURCE_DIR}/src/console/consolegeneratedfunctions.cpp")

##########################################################################
#
#   METROLOGY_LIBRARY target
#
#   METROLOGY_LIBRARY (as an INTERFACE library) does not build itself
#   but target(s) it depends on (such as METROLOGY_HEADERS) are followed
#   (transitively) in its place and trigger a rebuild when needed.
#

#add_library(${target} INTERFACE)
#target_include_directories(${target} INTERFACE Units)
#add_dependencies(${target} METROLOGY_HEADERS)
