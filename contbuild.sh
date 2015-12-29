#!/bin/bash

set -e

cc="$1"
if [ -z "$cc" ]; then
  echo "Usage: $0 compiler"
  exit 1
fi

if ! which $cc; then
  echo "no $cc installed"
  exit 1
fi

cpp_std="$2"

#if [ "$cc" = "g++-4.8" ]; then
#  USE_CC=$cc NO_CLEAR=true PRE_PROC=true ./test.sh \
#    type reflect_member_function || true
#fi

base_dir="/tmp/_build/$cc/$cpp_std"

if [ -e "$base_dir" ]; then
  rm -rf "$base_dir"
fi

echo "compiler: $cc"
echo "standard: $cpp_std"
$cc --version
USE_CC="$cc" USE_STD="$cpp_std" USE_CCACHE=true NO_CLEAR=true ./validate.sh
