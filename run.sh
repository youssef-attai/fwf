#!/usr/bin/env bash

# Compile
gcc -o fwif fwif.c -lX11 -ljson-c

# Run
python3 app.py

