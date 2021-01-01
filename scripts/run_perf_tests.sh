#!/usr/bin/env bash

set -x
set -e

path=$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)

echo "Build and run the perf tests ..."
cd $path/..
if [ ! -e cmake-build-debug ]; then
  mkdir -p cmake-build-debug
fi

cd $path/../cmake-build-debug
cmake .. -DFLAVOR=DEBUG
make perf_test
./perf_test "$@"

set +x
