#!/usr/bin/env bash

# This file is designed to be included by other script file
# Not recommended to run this file by itself

source_file_extensions=".c .h .cpp .hpp .cc .hh .cxx .inl"

function match_source_file_extension() {
  local file_name=$(basename "$1")
  local extension=".${file_name##*.}"
  local allowed_ext
  for allowed_ext in $source_file_extensions; do
    [[ "$allowed_ext" == "$extension" ]] && return 0;
  done
  return 1
}
