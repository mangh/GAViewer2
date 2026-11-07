# Garamon (Geometric Algebra Recursive and Adaptative Monster): a generator of
# C++ geometric algebra libraries, https://github.com/mangh/garamon (fork).
#
# The generator is fetched and built, and each algebra used by GAViewer is
# generated from its configuration file in this directory (<name>.conf) at
# build time. A generated algebra is a header-only library (it needs Eigen,
# which the generator project provides as the target Eigen3::Eigen):
#
#   garamon_<name>   interface library; #include <<name>/Mvec.hpp>
#
# To build against a local checkout of the fork (e.g. while working on it):
#
#   cmake ... -DFETCHCONTENT_SOURCE_DIR_GARAMON=<path to the fork>

message(DEBUG "Entering the Garamon subdirectory ...")

include(FetchContent)

FetchContent_Declare(
  garamon
  GIT_REPOSITORY https://github.com/mangh/garamon.git
  GIT_TAG        6ee8c1124ce63b4d33d6c31d456cfd7d6c5f33e1 # "Mvec: no implicit conversions to and from non-arithmetic types", Nov 6, 2026
  EXCLUDE_FROM_ALL # the generator is built only as a dependency of the generated algebras
)

set(GARAMON_BUILD_TESTS OFF) # the generator's own unit tests are not needed here
FetchContent_MakeAvailable(garamon)

# garamon_add_algebra(<name>)
#
# Generates the algebra described by <name>.conf (in this directory; its
# <namespace> must be <name>) into the build tree and creates the interface
# library garamon_<name>.
function(garamon_add_algebra name)
  set(conf "${CMAKE_CURRENT_SOURCE_DIR}/${name}.conf")
  set(output_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
  set(library_dir "${output_dir}/garamon_${name}")
  set(stamp "${library_dir}/src/${name}/Mvec.hpp")

  file(GLOB_RECURSE templates CONFIGURE_DEPENDS "${garamon_SOURCE_DIR}/data/*")

  add_custom_command(
    OUTPUT "${stamp}"
    COMMAND "${CMAKE_COMMAND}" -E rm -rf "${library_dir}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${output_dir}"
    COMMAND garamon_generator -c "${conf}" -t "${garamon_SOURCE_DIR}/data/" -o "${output_dir}/"
    DEPENDS "${conf}" ${templates} garamon_generator
    COMMENT "Generating the Garamon algebra ${name}"
    VERBATIM
  )
  add_custom_target(garamon_${name}_generate DEPENDS "${stamp}")

  add_library(garamon_${name} INTERFACE)
  target_include_directories(garamon_${name} SYSTEM INTERFACE "${library_dir}/src")
  target_link_libraries(garamon_${name} INTERFACE Eigen3::Eigen)
  target_compile_features(garamon_${name} INTERFACE cxx_std_20)
  add_dependencies(garamon_${name} garamon_${name}_generate) # consumers wait for the generated headers
endfunction()

garamon_add_algebra(e3ga)
garamon_add_algebra(p3ga) # Euclidean metric, unlike the fork's conf/p3ga.conf (see p3ga.conf)
garamon_add_algebra(c3ga) # basis order e1 e2 e3 no ni, as in Gaigen (see c3ga.conf)
garamon_add_algebra(c5ga) # basis order e1 ... e5 no ni, as in Gaigen (see c5ga.conf)

if(GAVIEWER_BUILD_TESTS)
  add_executable(garamon_smoke_e3ga "${CMAKE_CURRENT_SOURCE_DIR}/smoke_e3ga.cpp")
  target_link_libraries(garamon_smoke_e3ga PRIVATE garamon_e3ga)
  add_test(NAME garamon.smoke_e3ga COMMAND garamon_smoke_e3ga)
  set_tests_properties(garamon.smoke_e3ga PROPERTIES LABELS "garamon")
endif()
