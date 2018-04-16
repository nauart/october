#!/usr/bin/env sh

FILES=$(find ./include ./test/src ./benchmark/src -iname *.h -o -iname *.c -o -iname *.cc -o -iname *.hpp -o -iname *.cpp -type f)

if [ "$1" = "--fix" ]
then
  for FILE in $FILES; do clang-format -style=file -i $FILE; done
else
  for FILE in $FILES; do clang-format -style=file $FILE | diff $FILE -; done
fi
