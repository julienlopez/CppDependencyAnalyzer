#!/bin/bash

CC=gcc-9 && CXX=g++-9

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make VERBOSE=1
./unit_tests/unit_tests
