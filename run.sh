#!/bin/sh

clear

# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./libs/:$b
filePath=output/editor.out
make ./build/include/files.json
compiledb make -j 20 -s -w $filePath || exit 1

clear
#uncommit for waland debug info
# WAYLAND_DEBUG=1 $filePath

#uncommit for heaptrack profiling
# heaptrack -o profileData $filePath

# ./submodules/tracy/capture/build/unix/capture-release -a 127.0.0.1 -o trace
$filePath
echo "program exited with code $?"
