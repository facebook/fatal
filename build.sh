#!/bin/sh

set -e

if [ "$NO_CLEAR" != "true" ]; then
  ./lclear.sh >&2
fi

if [ -z "$USE_CC" ]; then
  for cc in clang++-3.5 g++-4.8 g++-4.9 clang++-3.4; do
    if which $cc; then
      USE_CC="$cc" NO_CLEAR=true "$0" "$@"
    fi
  done
else
  for f in `find ./fatal -name \*.cpp`; do
    USE_CC="$USE_CC" NO_CLEAR=true ./compile.sh "$f" "$@"
  done
fi
