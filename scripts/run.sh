#!/bin/bash

set -e

REPO=$(git rev-parse --show-toplevel)
cd "$REPO"

make clean
make
clear
./evolution > ./debug.log