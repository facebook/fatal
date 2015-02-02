#!/bin/sh

if [ -z "$USE_STD" ]; then
  export USE_STD="c++1y"
fi

file_name="demo/$1.cpp"

./compile.sh "$file_name" && rlwrap "/tmp/_bin/demo/$1" "$@"
