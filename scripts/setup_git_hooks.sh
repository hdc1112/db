#!/usr/bin/env bash

set -x
set -e

path=$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)

echo "Setting the pre-commit hook file ..."
cp $path/pre-commit $path/../.git/hooks/
echo "Done"

set +x
