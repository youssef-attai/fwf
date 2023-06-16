#!/usr/bin/env bash

# Compile
gcc -o fwif fwif.c -lX11 -ljson-c

# Environment variables
export PYTHONPATH=$(pwd)

# Run
python examples/dynamic_list_app.py

