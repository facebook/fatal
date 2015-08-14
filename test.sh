#!/bin/sh

set -e

if [ "$NO_CLEAR" != "true" ]; then
  ./lclear.sh >&2
fi

if [ -z "$1" ]; then
  if [ -z "$USE_CC" ]; then
    for cc in g++-4.8 clang++-3.7 g++-5 clang++-3.4; do
      for f in `find . -name \*_test.cpp`; do
        USE_CC="$cc" NO_CLEAR=true "$0" "$f" "$@"
      done
    done
  else
    for f in `find . -name \*_test.cpp`; do
      USE_CC="$USE_CC" NO_CLEAR=true "$0" "$f" "$@"
    done
  fi
elif [ -z "$2" ]; then
  test_basename="`basename "$1" .cpp`"
  if ! [ -e "$1" ]; then
    test_name="$1"
    test_path="fatal/test/${test_name}_test.cpp"
    if [ -e "$test_path" ]; then
      shift
      "$0" "$test_path"
      exit 0
    else
      echo "test $test_basename not found">&2
      exit 1
    fi
  fi
  PRE_PROC="$PRE_PROC" USE_CC="$USE_CC" NO_CLEAR=true DO_RUN=true ./compile.sh "$1"
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
