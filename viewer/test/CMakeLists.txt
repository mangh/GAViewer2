# Headless regression tests for GAViewer
#
# Every book figure (FIG_x_y.g) and every test script (scripts/<name>.g) is
# executed by the 'gaviewer_regress' harness, without any GUI, and the resulting
# state (console output, objects, their multivector coordinates and
# interpretations) is compared with a golden reference in viewer/test/golden.
#
#   Run the tests:                 ctest --test-dir <build-dir> [-R FIG_2_2] [--output-on-failure]
#   (Re)create golden references:  cmake --build <build-dir> --target regress_update_golden
#
# The actual output of each test is written to <build-dir>/viewer/test/actual,
# so a failing test can be inspected with any diff tool.

message(DEBUG "Entering the GAViewer regression tests subdirectory ...")

set(harness "gaviewer_regress")

add_executable(${harness} "${CMAKE_CURRENT_SOURCE_DIR}/regress.cpp")
target_link_libraries(${harness} PRIVATE gaviewer_core)

if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC")
  target_compile_definitions(${harness} PRIVATE _CRT_SECURE_NO_WARNINGS)
  target_compile_options(${harness} PRIVATE -W3)
elseif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "GNU")
  target_compile_options(${harness} PRIVATE -Wall)
endif()
if (USE_ASAN)
  target_compile_options(${harness} PRIVATE -fsanitize=address)
endif()
# same IPO/LTO setting as the core library it links to:
get_property(core_ipo TARGET gaviewer_core PROPERTY INTERPROCEDURAL_OPTIMIZATION)
if (core_ipo)
  set_property(TARGET ${harness} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# The sample figures are fetched (and patched) by the 'figures' subdirectory:
FetchContent_GetProperties(figs)
if (NOT figs_POPULATED)
  message(FATAL_ERROR "The sample figures must be fetched before the regression tests are configured")
endif()
add_dependencies(${harness} figures) # make sure the figures are patched

set(golden_dir "${CMAKE_CURRENT_SOURCE_DIR}/golden")
set(actual_dir "${CMAKE_CURRENT_BINARY_DIR}/actual")

file(GLOB figure_files LIST_DIRECTORIES false "${figs_SOURCE_DIR}/FIG_*.g")
list(SORT figure_files COMPARE NATURAL)

set(update_commands "")
foreach(figure_file ${figure_files})
  get_filename_component(figure "${figure_file}" NAME_WE)
  # console command; $<SEMICOLON>, because a plain ';' would split the command in a CMake list
  set(command "${figure}()$<SEMICOLON>")

  add_test(
    NAME "figure.${figure}"
    COMMAND ${harness}
      -I "${figs_SOURCE_DIR}"
      -o "${actual_dir}/${figure}.txt"
      -g "${golden_dir}/${figure}.txt"
      "${command}"
  )
  set_tests_properties("figure.${figure}" PROPERTIES TIMEOUT 60 LABELS "figures")

  list(APPEND update_commands
    COMMAND ${harness} -I "${figs_SOURCE_DIR}" -g "${golden_dir}/${figure}.txt" -u "${command}"
  )
endforeach()

# Test scripts (viewer/test/scripts/<name>.g): each defines a console function
# <name>() that exercises a part of the console language (e.g. all the built-in
# per-algebra functions in every model).
set(scripts_dir "${CMAKE_CURRENT_SOURCE_DIR}/scripts")
file(GLOB script_files LIST_DIRECTORIES false CONFIGURE_DEPENDS "${scripts_dir}/*.g")
list(SORT script_files)

foreach(script_file ${script_files})
  get_filename_component(script "${script_file}" NAME_WE)
  set(command "${script}()$<SEMICOLON>")

  add_test(
    NAME "script.${script}"
    COMMAND ${harness}
      -I "${scripts_dir}"
      -o "${actual_dir}/${script}.txt"
      -g "${golden_dir}/${script}.txt"
      "${command}"
  )
  set_tests_properties("script.${script}" PROPERTIES TIMEOUT 120 LABELS "scripts")

  list(APPEND update_commands
    COMMAND ${harness} -I "${scripts_dir}" -g "${golden_dir}/${script}.txt" -u "${command}"
  )
endforeach()

add_custom_target(regress_update_golden
  ${update_commands}
  DEPENDS ${harness}
  COMMENT "Regenerating the golden references in ${golden_dir}"
  VERBATIM
)
