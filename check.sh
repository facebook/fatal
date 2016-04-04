#!/bin/sh

set -e

USE_CC='clang++' ./test.sh "$@"
NO_CLEAR=true USE_CC='g++' ./test.sh "$@"
NO_CLEAR=true ./test.sh "$@"
