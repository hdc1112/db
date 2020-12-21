#!/usr/bin/env bash

# Input is a list of files to be checked

set -x
set -e
path=$(cd $(dirname ${BASH_SOURCE[0]}); pwd)

if [ -z $path; ]; then
  echo "Cannot get script path" >&2
  exit 1
fi

source $path/ensure-clang+llvm.sh

clang_tidy_bin=$LLVM_ROOT/bin/clang-tidy

cd $path/..
rm -rf build
mkdir -p build
cd build
cmake ..
cd $path/..

echo Checking files $@ ...

for file in $@; do
  $clang_tidy_bin -p build $file
done

set +x
