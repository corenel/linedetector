#!/usr/bin/env bash

sudo apt install -y libgflags-dev swig
mkdir build && cd build
cmake .. -DBUILD_PYTHON_BINDINGS=ON 
make
ln -sfn $PWD/build/_line_detector.so $HOME/.local/lib/python3.6/site-packages/
ln -sfn $PWD/build/line_detector.py $HOME/.local/lib/python3.6/site-packages/
