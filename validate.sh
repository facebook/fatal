#/bin/sh

set -e

skip_test=false
skip_build=false
skip_demo=false
while [ "$1" ]; do
  if [ "$1" = "notest" ]; then
    skip_test=true
  elif [ "$1" = "nobuild" ]; then
    skip_build=true
  elif [ "$1" = "nodemo" ]; then
    skip_demo=true
  fi

  shift
done

if [ "$NO_CLEAR" != "true" ]; then
  lclear.sh >&2
fi

if [ "$skip_test" != "true" ]; then
  NO_CLEAR=true ./test.sh
fi

if [ "$skip_build" != "true" ]; then
  NO_CLEAR=true ./build.sh
fi

if [ "$skip_demo" != "true" ]; then
  for demo in ytse_jam; do
    for cc in clang++-3.5 g++-4.9 clang++-3.4; do
      echo -n | USE_CC="$cc" NO_CLEAR=true ./demo.sh $demo
    done
  done
fi
