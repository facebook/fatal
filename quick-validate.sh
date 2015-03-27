#/bin/sh

set -e

if [ "$NO_CLEAR" != "true" ]; then
  ./lclear.sh >&2
fi

for cc in clang++-3.7; do
  NO_CLEAR=true USE_CC="$cc" ./validate.sh "$@"
done

for cc in g++-4.8 clang++-3.4 g++-4.9; do
  NO_CLEAR=true USE_CC="$cc" ./validate.sh nobuild "$@"
done
