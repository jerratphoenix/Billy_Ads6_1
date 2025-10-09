#!/bin/sh

set -e

if ! command -v docker > /dev/null; then
  echo "Please install Docker"
  exit 1
fi

docker build --force-rm --target builder -t scm-i2c-memory-map-build .