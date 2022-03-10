#! /bin/bash

SOURCE_DIR="source"
TARGET_DIR="target"

cmake -G Ninja -S $SOURCE_DIR -B $TARGET_DIR -DCMAKE_BUILD_TYPE=Release $1
cmake --build $TARGET_DIR --target stackmachine
