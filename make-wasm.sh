#!/bin/bash
cd `dirname $0`
output_path=./html/dist
test -d ${output_path} || mkdir -p ${output_path}
export EMCC_WASM_BACKEND=1
em++ -o ${output_path}/libsovol-wasm.js ./libsovol-wasm.so --bind -s \
  TOTAL_MEMORY=200MB -s ALLOW_TABLE_GROWTH=1