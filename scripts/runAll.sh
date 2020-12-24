#!/usr/bin/env bash

set -x
set -e

path=$(cd $(dirname ${BASH_SOURCE[0]}); pwd)

# This is a slow script but designed to be thorough

echo "Checking libfmt installation ..."
echo "Checking gtest installation ..."
echo "Init all submodules ... "
git submodule update --init --recursive

echo "Build and run the unit tests ..."
cd $path/..
if [ ! -e cmake-build-debug ]; then
  mkdir -p cmake-build-debug
fi

cd $path/../cmake-build-debug
cmake .. -DFLAVOR=DEBUG
make clean
make unit_test
./unit_test

set +x
