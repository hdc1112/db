#!/usr/bin/env bash

set -x
set -e

path=$(cd $(dirname ${BASH_SOURCE[0]}); pwd)

echo "This is a slow script but designed to be thorough"

echo "Init all submodules ... "
git submodule update --init --recursive

echo "Checking libfmt installation ..."
echo "Checking gtest installation ..."

echo "Build and run the unit tests ..."
cd $path/..
if [ ! -e cmake-build-debug ]; then
  mkdir -p cmake-build-debug
fi

cd $path/../cmake-build-debug
cmake .. -DFLAVOR=DEBUG
make unit_test
./unit_test

echo "Build and run the perf tests ..."
cd $path/..
if [ ! -e cmake-build-debug ]; then
  mkdir -p cmake-build-debug
fi

cd $path/../cmake-build-debug
cmake .. -DFLAVOR=DEBUG
make perf_test
./perf_test

set +x
