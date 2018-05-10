#!/usr/bin/env sh

SRC_DIR=$1
OBJECT_DIR=test/CMakeFiles
COVERAGE_DIR=test/coverage

find . -name *.gcda -type f -delete
rm -rf $COVERAGE_DIR
mkdir $COVERAGE_DIR

make
lcov --no-external --capture --initial --base-directory $SRC_DIR --directory $OBJECT_DIR --output-file $COVERAGE_DIR/lcov_initial.info

make test
lcov --no-external --capture --base-directory $SRC_DIR --directory $OBJECT_DIR --output-file $COVERAGE_DIR/lcov_processed.info
lcov --add-tracefile $COVERAGE_DIR/lcov_initial.info --add-tracefile $COVERAGE_DIR/lcov_processed.info --output-file $COVERAGE_DIR/lcov_result.info

genhtml $COVERAGE_DIR/lcov_result.info --output-directory=$COVERAGE_DIR/report
