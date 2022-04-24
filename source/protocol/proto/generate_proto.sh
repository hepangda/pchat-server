#!/bin/bash
for filename in $(find . -name "*.proto")
do
  $1 $filename --cpp_out=../ -I=./
done

