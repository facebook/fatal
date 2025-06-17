#!/bin/sh

. ./scripts.inc

if [ -z "$USE_STD" ]; then
  export USE_STD="$(flag_std_cpp17 $default_compiler)"
fi

file_name="demo/$1.cpp"
shift

DO_RUN=true ./compile.sh "$file_name" "$@"
