#!/bin/bash -e

basedir=$(dirname $(realpath $0))

for test in $(find $basedir -name "*_test.sh");do
  echo "Running $test"
  eval $test
done