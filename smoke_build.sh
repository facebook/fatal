#!/bin/bash

. ./scripts.inc

set -e

if [ "$1" ]; then
  full_compilers_list="$@"
fi

if [ -z "$BE_LENIENT" ]; then
  export BE_LENIENT=true
fi

for cc in $full_compilers_list; do
  for std in `flag_std_cpp11 $cc` `flag_std_cpp14 $cc` `flag_std_cpp17 $cc`; do
    echo "## full build $cc $std"
    USE_CC="$cc" USE_STD="$std" USE_CCACHE=true NO_CLEAR=true ./build.sh
    echo
    echo
  done
done
