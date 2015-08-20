#!/bin/bash

set -e

for cc in g++-4.8 g++-4.9 g++-5 clang-3.4 clang-3.5 clang-3.6 clang-3.7 clang-3.8; do
  if which $cc; then
    echo "Package: $cc"
    dpkg -s $cc | grep '^Status: '
  fi
done

for cc in g++-4.8 g++-4.9 g++-5; do
  if which $cc; then
    $cc --version
    # TODO: split prefix_tree_benchmark into several benchmarks to avoid
    #       breaking contbuild and remove the `nobuild` flag, then remove
    #       this for loop and let `validate.sh` figure the compilers out
    USE_CC=$cc NO_CLEAR=true ./validate.sh nobuild
  else
    echo "no $cc installed, skipping validation"
  fi
done

for cc in clang++-3.8 clang++-3.4 clang++ clang++-3.5 clang++-3.6 clang++-3.7; do
  if which $cc; then
    $cc --version
    # TODO: split prefix_tree_benchmark into several benchmarks to avoid
    #       breaking contbuild and remove the `nobuild` flag
    USE_CC=$cc NO_CLEAR=true ./validate.sh nobuild
  else
    echo "no $cc installed, skipping validation"
  fi
done
