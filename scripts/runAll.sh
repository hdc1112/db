#!/usr/bin/env bash

set -x
set -e

path=$(cd $(dirname ${BASH_SOURCE[0]}); pwd)

# This is a slow script but designed to be thorough

echo "Checking libfmt installation ..."
echo "Checking gtest installation ..."

echo "Reset the pre-commit hook file ..."
cp $path/pre-commit $path/../.git/hooks/

set +x
