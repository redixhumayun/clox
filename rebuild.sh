#!/bin/bash

emmake make clean && 
emmake make wasm &&
python3 -m http.server
