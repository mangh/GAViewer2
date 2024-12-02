##########################################################################
#
#   patch(
#     <source_dir>  # source directory containing file to patch
#     <file_name>   # name of the file to patch
#     <regex>       # regex for what must be in the patched (revised) file
#     <patch_dir>   # patch directory containing previously prepared, patched file
#   )
#   
#   The function copies the original (unpatched) file:
#   
#     <source_dir>/<file_name> => <patch_dir>/<file_name>.original
#   
#   and then copies in its place the previously prepared, patched file:
#   
#     <patch_dir>/<file_name>.patched => <source_dir>/<file_name>.
#
#

function(patch source_dir file_name regex patch_dir)

  list(APPEND CMAKE_MESSAGE_INDENT "(patch) ")

  set(_source_file "${source_dir}/${file_name}")

  if (NOT EXISTS "${_source_file}")
    message(WARNING "The ${_source_file} file (to be patched) does not exist.")

  else()

    # Checks whether the source file is already patched
    # (the regex variable specifies what should be in the revised file)
    file(STRINGS "${_source_file}" _patch_applied REGEX ${regex})
    if (_patch_applied)

      message(DEBUG "The ${_source_file} file has already been patched.")

    else()

      set(_patch_file "${patch_dir}/${file_name}")

      # save the original (unpatched) file ...
      if (NOT EXISTS "${_patch_file}.original")
        file(COPY_FILE "${_source_file}" "${_patch_file}.original")
      endif()

      # ... and put the patched file instead
      file(COPY_FILE "${_patch_file}.patched" "${_source_file}")

      message(DEBUG "The ${_source_file} file has been patched.")

    endif()
    
  endif()

  list(POP_BACK CMAKE_MESSAGE_INDENT)

endfunction()