#!/usr/bin/env bash

# Must be run from the repository root.

set -o nounset
set -o errexit
set -o pipefail
shopt -s nullglob

cd etc/deps/avro/lang/c++

# We can't simply run `./build.sh install` because of a deprecated function
# used in tests. We must first generate the Makefile by running a fake command,
# then only make the shared library we are interested in.
./build.sh foo || : # Hack.
cd build
make avrocpp
