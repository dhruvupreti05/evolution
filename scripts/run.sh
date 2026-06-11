#!/bin/bash

REPO=$(git rev-parse --show-toplevel)
cd "$REPO"

make clean && make && ./evolution