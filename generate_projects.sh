#!/bin/bash
mkdir build
pushd build
cmake .. -G Ninja
popd
mkdir build_debug
pushd build_debug
cmake -DCMAKE_BUILD_TYPE=Debug .. -G Ninja
popd
 