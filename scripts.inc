#!/bin/sh

. ./compilers.conf

short_compilers_list="$highest_clang $lowest_gcc $highest_gcc $lowest_clang"
full_compilers_list="$short_compilers_list $alt_clang_list $alt_gcc_list"

default_clang="$highest_clang"
default_gcc="$highest_gcc"

default_compiler="$default_clang"

system_clang="clang++"
system_gcc="g++"

lclear() {
  if [ "$NO_CLEAR" != "true" ]; then
    tmux clear-history || true
    clear >&2
  fi
}
