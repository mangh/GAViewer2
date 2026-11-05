# FLTKEditor executable
#

message(DEBUG "Entering the FLTK Editor subdirectory ...")

# Add executable target to the project using the specified source files:
set(target "FLTKEditor")

list(APPEND _source_files
  "editor.cxx"
  "editor_features.cxx"
  "file_handling.cxx"
  "find_replace.cxx"
  "main_window.cxx"
  "menu.cxx"
  "text_editor.cxx"
)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
  list(APPEND _source_files
    "fl_call_main.cxx"  # WinMain entry point (Windows specific)
  )
endif()

add_executable(${target} ${_source_files})

# # Add resources (icon)
# if (CMAKE_SYSTEM_NAME MATCHES "Windows")
#   target_sources(${target} PRIVATE "${gavr_SRC_PATH}/gaviewer.rc")
# endif()

##########################################################################
#
#   COMPILE DEFINITIONS & OPTIONS
#

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
  
  target_compile_definitions(${target}
    PRIVATE WIN32             # application being built for Windows
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

  target_compile_definitions(${target} PRIVATE
    # deprecation warnings for the older, less secure functions
    _CRT_SECURE_NO_WARNINGS   
  )
  target_compile_options(${target} PRIVATE
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
#   NOTE: I have not been able to get Address Sanitizer to work on Windows
#        (regardless of the compiler used)!
#

if(USE_ASAN)
  # if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC")
  #   # _DISABLE_*_ANNOTATION definitions are (were?) required to avoid error:
  #   #
  #   #    lld-link: error: /INFERASANLIBS is not allowed in .drectve
  #   #
  #   target_compile_definitions(${target} PRIVATE
  #     _DISABLE_STRING_ANNOTATION
  #     _DISABLE_VECTOR_ANNOTATION
  #   )
  # endif()
  # Note: MSVC accepts both slash (/) and dash (-) options
  target_compile_options(${target} PRIVATE -fsanitize=address)
  # target_link_options(${target} PRIVATE -fsanitize=address)
endif()

##########################################################################
#
#   LINK OPTIONS & LIBRARIES
#

target_link_libraries(${target}
  PRIVATE fltk::png
  PRIVATE fltk::gl
  PRIVATE fltk::fltk
)

# Take advantage of interprocedural optimization (IPO/LTO), if available:
include(CheckIPOSupported OPTIONAL RESULT_VARIABLE check_ipo_module_found)
if(check_ipo_module_found)
  check_ipo_supported(
    RESULT LTO_OPTIMIZATION_SUPPORTED
    OUTPUT LTO_OPTIMIZATION_ERROR
    LANGUAGES C CXX
  )
  if(LTO_OPTIMIZATION_SUPPORTED)
    set_property(TARGET ${target} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  else()
    message(DEBUG "Interprocedural optimization (IPO/LTO) is not supported: ${LTO_OPTIMIZATION_ERROR}")
  endif()
endif()

# Check libraries usage:
set_target_properties(${target} PROPERTIES LINK_WHAT_YOU_USE ON)

# ##########################################################################
# #
# #   INSTALL
# #
# 
# install(
#   TARGETS ${target} 
#   DESTINATION ${CMAKE_INSTALL_BINDIR}
#   COMPONENT GAViewer
# )

#   After compilation, the application can be installed using a command
#   similar to the following:
#
#     cmake --install ~/GAViewer/build [--component GAViewer] --prefix /my/install/prefix
#
#   For example, on Windows, it could be:
#
#     cmake --install %USERPROFILE%\source\repos\GAViewer\build [--component GAViewer] --prefix %LOCALAPPDATA%\Programs\GAViewer
#
#   If you omit the '--component' parameter, CMake will install everything i.e.
#   the GAViewer executable, sample figures, but also FLTK (libraries and headers).
#
