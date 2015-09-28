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

if [ "$cc" = "g++-4.8" ]; then
  USE_CC=$cc NO_CLEAR=true PRE_PROC=true ./test.sh type reflect_member_function || true
fi

echo "compiler: $cc"
$cc --version
# TODO: split prefix_tree_benchmark into several benchmarks to avoid
#       breaking contbuild and remove the `nobuild` flag, then remove
#       this for loop and let `validate.sh` figure the compilers out
USE_CC=$cc NO_CLEAR=true ./validate.sh nobuild
