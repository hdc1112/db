#!/usr/bin/env bash

set -x
set -e

path=$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)

if [ -z $path ]; then
  echo "Cannot get script path" >&2
  exit 1
fi

LLVM_HOME=$path/../../db-tools/llvm

if [ ! -d $LLVM_HOME ]; then
  echo Creating directory $LLVM_HOME
  mkdir -p $LLVM_HOME
fi

LLVM_VERSION=11.0.0

if [ "$(uname)" == "Darwin" ]; then
  OS_STRING=apple-darwin
else
  OS_STRING=linux-gnu-ubuntu-16.04
fi

LLVM_NAME=clang+llvm-$LLVM_VERSION-x86_64-$OS_STRING
LLVM_TAR_BALL=$LLVM_NAME.tar.xz
LLVM_LINK=https://github.com/llvm/llvm-project/releases/download/llvmorg-$LLVM_VERSION/$LLVM_TAR_BALL

LLVM_ROOT=$LLVM_HOME/$LLVM_NAME

if [ ! -f $LLVM_ROOT/bin/clang ]; then
  echo Downloading LLVM ...

  pushd $LLVM_HOME

  wget $LLVM_LINK
  tar -xf $LLVM_TAR_BALL
  rm $LLVM_TAR_BALL

  popd
fi

$LLVM_ROOT/bin/clang --version

set +x
