#!/usr/bin/env bash

set -x
set -e

path=$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)

cd $path/..

cd $path/..
if [ ! -e cmake-build-debug ]; then
  mkdir -p cmake-build-debug
fi

cd $path/../cmake-build-debug
cmake .. -DFLAVOR=DEBUG
make unit_test
./unit_test "$@"

set +x
