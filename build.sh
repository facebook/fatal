#!/bin/sh

. ./scripts.inc

set -e

lclear

if [ -z "$USE_CC" ]; then
  for cc in $full_compilers_list; do
    if which $cc; then
      USE_CC="$cc" NO_CLEAR=true "$0" "$@"
    fi
  done
else
  if [ "$EXCLUDE_BENCHMARKS" = true ]; then
    for f in `find fatal -name \*.cpp|grep -v '/.*/.*benchmark/'`; do
      CC_OPT="-O0" USE_CC="$USE_CC" NO_CLEAR=true ./compile.sh "$f" "$@"
    done
  else
    for f in `find fatal -name \*.cpp`; do
      CC_OPT="-O0" USE_CC="$USE_CC" NO_CLEAR=true ./compile.sh "$f" "$@"
    done
  fi
fi
