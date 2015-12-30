#/bin/sh

set -xe

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

if [ "$USE_STD" = "c++11" ] || [ "$USE_STD" = "c++0x" ]; then
  skip_demo=true
fi

run_validation() {
  cc="$1"

  if ! which "$cc"; then
    return
  fi

  if [ "$NO_CLEAR" != "true" ]; then
    ./lclear.sh >&2
  fi

  if [ "$skip_build" != "true" ]; then
    CC_OPT="-O0" USE_CC="$cc" NO_CLEAR=true ./build.sh
  fi

  if [ "$skip_test" != "true" ]; then
    CC_OPT="-O0" USE_CC="$cc" NO_CLEAR=true ./test.sh
  fi

  if [ "$skip_demo" != "true" ] && [ "$cc" != "g++-4.8" ]; then
    for demo in ytse_jam; do
      echo -n | CC_OPT="-O0" USE_CC="$cc" NO_CLEAR=true ./demo.sh $demo
    done
  fi
}

if [ "$USE_CC" ]; then
  run_validation "$USE_CC"
else
  for cc in clang++-3.8 g++-4.8 g++-5 clang++-3.4 clang++-3.5 clang++-3.6 clang++-3.7 g++-4.9; do
    run_validation "$cc"
  done
fi
