#!/bin/sh

set -e

if [ "$NO_CLEAR" != "true" ]; then
  lclear.sh >&2
fi

if [ -z "$1" ]; then
  set -x
  for cc in clang++-3.5 g++-4.8 g++-4.9 clang++-3.4; do
    for f in `find . -name \*_test.cpp`; do
      USE_CC="$cc" NO_CLEAR=true "$0" "$f" "$@"
    done
  done
elif [ -z "$2" ] && [ -e "$1" ]; then
  test_path="`basename "$1" .cpp`"
  if ! [ -e "$1" ]; then
    echo "test $test_path not found">&2
    exit 1
  fi
  USE_CC="$USE_CC" NO_CLEAR=true DO_RUN=true ./compile.sh "$1"
elif [ "$1" ] && [ "$2" ]; then
  while [ "$1" ] && [ "$2" ]; do
    dir_name="$1"
    test_name="$2"
    shift
    shift
    "$0" "fatal/$dir_name/test/${test_name}_test.cpp"
  done
else
  echo "no valid tests given" >&2
fi
