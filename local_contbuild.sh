#!/bin/bash

. ./scripts.inc

set -e

for cc in $full_compilers_list; do
  for std in `flag_std_cpp14 $cc` `flag_std_cpp17 $cc`; do
    echo "## contbuild $cc $std"
    EXCLUDE_BENCHMARKS=true ./contbuild.sh "$cc" "$std"
    echo
    echo
  done
done
