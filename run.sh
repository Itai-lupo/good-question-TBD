#!/bin/sh

clear

a=$(find ./DLLS -type d)
b=""
for i in $(echo $a | tr " " "\n")
do
    b+=$i:
done

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./libs/:$b
filePath=output/editor.out
make -j 14 -s -w $filePath || exit 1
clear
$filePath

echo "program exited with code $?"