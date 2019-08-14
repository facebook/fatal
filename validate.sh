# shellcheck disable=SC1113
#/bin/sh

. ./scripts.inc

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

run_validation() {
  cc="$1"

  if ! which "$cc"; then
    return
  fi

  lclear

  if [ "$skip_build" != "true" ]; then
    CC_OPT="-O0" USE_CC="$cc" NO_CLEAR=true ./build.sh
  fi

  if [ "$skip_test" != "true" ]; then
    CC_OPT="-O0" USE_CC="$cc" NO_CLEAR=true ./test.sh
  fi

  if [ "$skip_demo" != "true" ]; then
    for demo in ytse_jam; do
      echo -n | CC_OPT="-O0" USE_CC="$cc" NO_CLEAR=true ./demo.sh $demo
    done
  fi
}

if [ "$USE_CC" ]; then
  run_validation "$USE_CC"
else
  for cc in $full_compilers_list; do
    run_validation "$cc"
  done
fi
