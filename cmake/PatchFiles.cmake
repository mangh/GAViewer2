##########################################################################
#
#   Applying patches (via the git tool) to a single file:
#
#     patch_file(
#       <patch_dir>   # patch directory (containing patch files: *.original, *.patch, *.revised)
#       <root_dir>    # value of the “--directory” option to be used in the “git apply” command
#       <file_name>   # file (relative path) to be patched
#     )
#
#   Applying patches (via the git tool) to the list of files:
#
#     patch_files(
#       <patch_dir>   # patch directory (containing patch files: *.original, *.patch, *.revised)
#       <file_list>   # list of files (relative paths) to be patched
#     )
#
#   NOTE: To generate a patch for a <file> file, do the following:
#
#     cd <subdir>
#     cp ./<relative_path_to>/<file>.revised ../build/_deps/<subdir-src>/<relative_path_to>/<file>
#     cp ./<relative_path_to>/<file>.original ./<relative_path_to>/<file>
#     git diff --no-index --output=./<relative_path_to>/<file>.patch --patch ./<relative_path_to>/<file> ../build/_deps/<subdir-src>/<relative_path_to>/<file>
#

function(get_patch_root patch_dir root_dir)
  cmake_path(GET patch_dir PARENT_PATH _parent_dir)
  string(REPLACE "${_parent_dir}" "" _slash_subdir ${patch_dir})
  string(REPLACE "/" "" _subdir ${_slash_subdir})
  set(${root_dir} "${_subdir}" PARENT_SCOPE)
endfunction()

function(patch_file patch_dir root_dir file_name)

  set(source_file "${patch_dir}/${file_name}")
  set(original_file "${source_file}.original")
  set(diff_file "${source_file}.patch")

  if(NOT EXISTS "${original_file}")
    message(FATAL_ERROR "${original_file} file does NOT exists.")
  elseif(NOT EXISTS "${diff_file}")
    message(FATAL_ERROR "${diff_file} file does NOT exists.")
  else()

    list(APPEND CMAKE_MESSAGE_INDENT "(patch) ")

    # Restore the original a/file (in the patch directory):
    file(COPY_FILE "${original_file}" "${source_file}")

    set(adverb " ")

    # Apply the patch:
    execute_process(
      COMMAND ${GIT_EXECUTABLE} apply --unsafe-paths --directory=${root_dir} --whitespace=nowarn ${diff_file}
      WORKING_DIRECTORY "${patch_dir}"
      # COMMAND_ECHO STDOUT
      COMMAND_ERROR_IS_FATAL ANY
      RESULT_VARIABLE exit_code
    )

    if(${exit_code})
      set(adverb " NOT ")
    endif()
    message(DEBUG "The ${diff_file} file has${adverb}been applied (git exit code = ${exit_code}).")

    list(POP_BACK CMAKE_MESSAGE_INDENT)

  endif()

endfunction()

function(patch_files patch_dir files_to_path)

  get_patch_root(${patch_dir} patch_root)

  foreach(file ${files_to_patch})
    patch_file("${patch_dir}" "${patch_root}" "${file}")
  endforeach(file )

endfunction()
