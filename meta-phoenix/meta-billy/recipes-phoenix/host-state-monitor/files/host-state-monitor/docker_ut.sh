#!/bin/sh

set -e

if ! command -v docker > /dev/null; then
  echo "Please install Docker"
  exit 1
fi

docker build --force-rm --target unit-tests -t scm-i2c-memory-map-ut .

docker run --rm scm-i2c-memory-map-ut:latest