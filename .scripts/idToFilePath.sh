#!/bin/bash

cat ./.build/include/files.json | grep -o -P "\[\"[a-zA-Z0-9\/\.]*\", $1\]"
