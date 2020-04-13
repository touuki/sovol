#!/bin/bash

export EMCC_WASM_BACKEND=1
em++ -o libsovol-wasm.js libsovol-wasm.so --bind -s TOTAL_MEMORY=200MB -s \
ALLOW_TABLE_GROWTH=1