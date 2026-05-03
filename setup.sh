#!/bin/bash
set -e

echo "Configuring all presets..."

cmake --preset x64-debug
cmake --preset x64-release
cmake --preset x86-debug
cmake --preset x86-release

echo "Done."