#!/usr/bin/env bash

set -x
set -e

path=$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)
repo_root=$path/../

pushd $repo_root

while read tilde_file; do
  rm -f $tilde_file
done < <(find . -type f -name "*~" )

popd

set +x
