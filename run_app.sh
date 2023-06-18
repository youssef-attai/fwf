#!/usr/bin/env bash

# Environment variables
export PYTHONPATH=$(pwd)
export FWIF_WRITE_PIPE=/tmp/fwif_write_pipe
export FWIF_READ_PIPE=/tmp/fwif_read_pipe

# Run
python examples/dynamic_list_app.py

