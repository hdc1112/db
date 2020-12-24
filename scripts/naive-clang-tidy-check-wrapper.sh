#!/usr/bin/env bash

# This script file gets the changed files and pipe it to another script
# The changed files are got by: checking current commit (excluding files in index and working directory)
# and the main/master branch

set -x
set -e
path=$(cd $(dirname ${BASH_SOURCE[0]}); pwd)

if [ -z "$path" ]; then
  echo "Cannot get script path" >&2
  exit 1
fi

cd $path/..

source $path/file-utility.sh

# TODO: this code does not work if in main branch
git diff --name-only HEAD main | \
  while read modified_file; do
    if match_source_file_extension "$modified_file" && [ -f "$modified_file" ]; then
      echo $modified_file
    else 
      echo Skipping clang-tidy check for "$modified_file" >&2
    fi
  done | xargs $path/naive-clang-tidy-check.sh

set +x

