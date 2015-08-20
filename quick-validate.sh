#/bin/sh

set -e

if [ "$NO_CLEAR" != "true" ]; then
  ./lclear.sh >&2
fi

for cc in clang++-3.8; do
  NO_CLEAR=true CC_OPT="-O0" USE_CC="$cc" ./validate.sh "$@"
done

for cc in g++-4.8 clang++-3.4 g++-5; do
  NO_CLEAR=true CC_OPT="-O0" USE_CC="$cc" ./validate.sh nobuild "$@"
done
