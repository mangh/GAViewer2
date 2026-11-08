# Round trip of a book figure through a .geo file (the tests geo.FIG_x_y, see CMakeLists.txt):
#
#   cmake -DHARNESS=<gaviewer_regress> -DFIGS=<figures dir> -DFIGURE=<FIG_x_y> -DWORK=<dir>
#         [-DIGNORE=<name>|<name>...] -P geo_roundtrip.cmake
#
# 1. runs the figure (the figure scripts loaded) and saves the state to <WORK>/<FIGURE>.geo, as
#    File > Save of the GUI does (gaviewer_regress -s)
# 2. opens that file in a new process, without the figure scripts (as after a restart of
#    GAViewer), and compares its objects with those of the run (gaviewer_regress -l ... -c ...);
#    the objects in IGNORE (known differences, see geo_roundtrip_known.txt) are not compared

foreach(var HARNESS FIGS FIGURE WORK)
  if (NOT DEFINED ${var})
    message(FATAL_ERROR "geo_roundtrip.cmake: ${var} is not set")
  endif()
endforeach()

set(geo "${WORK}/${FIGURE}.geo")
set(run_dump "${WORK}/${FIGURE}.run.txt")
file(MAKE_DIRECTORY "${WORK}")

execute_process(
  COMMAND "${HARNESS}" -I "${FIGS}" -s "${geo}" -o "${run_dump}" "${FIGURE}();"
  RESULT_VARIABLE result
)
if (NOT result EQUAL 0)
  message(FATAL_ERROR "running ${FIGURE} and saving it to '${geo}' failed (${result})")
endif()

# (the names are separated by '|': a ';' would split the -D argument of the test command)
set(ignore_args "")
if (DEFINED IGNORE)
  string(REPLACE "|" ";" ignore_list "${IGNORE}")
  foreach(name IN LISTS ignore_list)
    list(APPEND ignore_args -i "${name}")
  endforeach()
endif()

execute_process(
  COMMAND "${HARNESS}" -l "${geo}" -c "${run_dump}" ${ignore_args} -o "${WORK}/${FIGURE}.open.txt"
  RESULT_VARIABLE result
)
if (NOT result EQUAL 0)
  message(FATAL_ERROR "the objects of ${FIGURE} differ after the round trip through '${geo}' (${result})")
endif()
