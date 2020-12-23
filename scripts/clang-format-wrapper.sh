#!/usr/bin/env bash

set -x
set -e 
path=$(cd $(dirname ${BASH_SOURCE[0]}); pwd)

if [ -z $path ]; then
  echo "Cannot get script path" >&2
  exit 1
fi

source $path/ensure-clang+llvm.sh

source $path/file-utility.sh

clang_format_bin=${LLVM_ROOT}/bin/clang-format

style_args="-style=file -assume-filename=$path/../.clang-format"

function format_file() {
  file="$1"
  echo -n " $file: "
  if match_source_file_extension "$file"; then
    echo "applying clang-format"
    $clang_format_bin -i $style_args $file || {
      echo "Error executing $clang_format_bin"
      exit 1
    }
  else
    echo "skipping clang-format"
  fi
}

if [ "$#" -lt 1 ]; then
  echo "Must specify one file in the argument list" >&2
  exit 1
fi

for file in "$@"; do
  format_file "$file"
done

