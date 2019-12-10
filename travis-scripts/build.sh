#!/bin/bash

CC=gcc-8 && CXX=g++-8

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make VERBOSE=1
./unit_tests/unit_tests
