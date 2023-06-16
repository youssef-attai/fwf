#!/usr/bin/env bash

# Compile
gcc -o fwif fwif.c -lX11 -ljson-c

# Environment variables
export PYTHONPATH=$(pwd)

# Run
python3 app.py

