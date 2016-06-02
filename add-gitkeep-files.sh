#!/bin/sh

find . -name .git -prune ! -name .git -o -type d -empty \
     -exec touch {}/.gitkeep \;
