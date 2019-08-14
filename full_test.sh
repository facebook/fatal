#!/bin/bash

. ./scripts.inc

set -e

lclear

for cc in $full_compilers_list; do
  for std in `flag_std_cpp14 $cc` `flag_std_cpp17 $cc`; do
    NO_CLEAR=true USE_CC="$cc" USE_STD="$std" ./test.sh "$@"
  done
done
