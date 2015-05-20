#!/bin/sh

set -e

if [ "$NO_CLEAR" != "true" ]; then
  ./lclear.sh >&2
fi

if [ -z "$USE_CC" ]; then
  for cc in clang++-3.7 g++-4.8 g++-5 clang++-3.4 clang++-3.5 clang++-3.6 g++-4.9; do
    if which $cc; then
      USE_CC="$cc" NO_CLEAR=true "$0" "$@"
    fi
  done
else
  for f in `find ./fatal -name \*.cpp`; do
    CC_OPT="-O0" USE_CC="$USE_CC" NO_CLEAR=true ./compile.sh "$f" "$@"
  done
fi
