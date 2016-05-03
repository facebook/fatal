#!/bin/sh

. ./scripts.inc

set -e

USE_CC="$system_clang" ./test.sh "$@"
NO_CLEAR=true USE_CC="$system_gcc" ./test.sh "$@"
NO_CLEAR=true ./test.sh "$@"
