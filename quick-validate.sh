# shellcheck disable=SC1113
#/bin/sh

. ./scripts.inc

set -e

lclear

no_build_flag=""
for cc in $short_compilers_list; do
  NO_CLEAR=true CC_OPT="-O0" USE_CC="$cc" ./validate.sh "$no_build_flag" "$@"
  if [ -z "$no_build_flag" ]; then
    no_build_flag="nobuild"
  fi
done
