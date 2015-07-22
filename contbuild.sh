#!/bin/bash

set -e

for cc in g++-4.8 g++-4.9 g++-5 clang-3.4 clang-3.5 clang-3.6 clang-3.7; do
  if which $cc; then
    echo "Package: $cc"
    dpkg -s $cc | grep '^Status: '
    dpkg -L $cc | grep '/bin/'
  fi
done

for cc in g++-4.8 g++-4.9 g++-5; do
  if which $cc; then
    $cc --version
    USE_CC=$cc NO_CLEAR=true ./validate.sh nobuild
  else
    echo "no $cc installed, skipping validation"
  fi
done

for cc in clang++-3.7 clang++-3.4 clang++ clang++-3.5 clang++-3.6; do
  if which $cc; then
    $cc --version
    USE_CC=$cc NO_CLEAR=true ./validate.sh
  else
    echo "no $cc installed, skipping validation"
  fi
done
