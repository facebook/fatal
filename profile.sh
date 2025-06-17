#!/bin/sh

. ./scripts.inc

set -e

if [ -z "$1" ]; then
  echo "Usage: $0 source_file"
  exit 1
fi

if [ -z "$USE_STD" ]; then
  export USE_STD="c++17"
fi

export USE_CC="templight"

file_name="$1"
shift
base_dir="/tmp/_build/$USE_CC/$USE_STD"
out_dir="$base_dir/`dirname "$file_name"`"

if [ ! -d "$out_dir" ]; then
  mkdir -p "$out_dir"
fi

out_path="$out_dir/`basename "$file_name" .cpp`"
out_binary="${out_path}.o"
out_prof="${out_binary}.memory.trace.pbf"

lclear

echo -n "started: "; date
set -x
templight++ -Xtemplight -profiler -Xtemplight -memory -Xtemplight -ignore-system -o "$out_binary" -O2 -Wall -Werror "-std=$USE_STD" -I . -c "$file_name" 2>&1
set +x
echo -n "finished: "; date
echo "results written to $out_prof"
