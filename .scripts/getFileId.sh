#!/bin/bash

cat ./.build/include/files.json | grep -o -P "($1).+?\"[^\"]*, \d*"
