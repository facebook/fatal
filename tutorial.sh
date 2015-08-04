#!/bin/sh

while [ "$1" ]; do
  ./demo.sh tutorial/$1
  shift
done
