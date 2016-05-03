#!/bin/sh

. ./scripts.inc

if [ -z "$USE_STD" ]; then
  export USE_STD="c++1y"
fi

file_name="demo/$1.cpp"
shift

DO_RUN=true ./compile.sh "$file_name" "$@"
