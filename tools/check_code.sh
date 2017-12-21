#!/usr/bin/env sh

cppcheck -q --force --enable=all --inconclusive --suppress=missingIncludeSystem --suppress=unusedFunction -I ./include ./include/*.h
