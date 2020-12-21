macro(file_glob_recurse OUT_VAR)
    file(GLOB_RECURSE ${OUT_VAR} CONFIGURE_DEPENDS ${ARGN})
endmacro()
