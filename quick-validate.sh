#/bin/sh

set -e

for cc in clang++-3.4 g++-4.8; do
  if [ "$NO_CLEAR" != "true" ]; then
    lclear.sh >&2
  fi

  NO_CLEAR=true USE_CC="$cc" ./validate.sh nobuild "$@"
done
