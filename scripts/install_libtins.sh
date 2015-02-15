#!/bin/env bash

set -e

libtins_v=3.1

mkdir tmp
cd tmp
curl -L "https://github.com/mfontanini/libtins/archive/v${libtins_v}.zip" >libtins.zip
unzip libtins.zip
cd "libtins-${libtins_v}"
mkdir build
cd build
cmake ../
make
sudo make install
