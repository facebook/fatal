#!/bin/bash

set -xe

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
