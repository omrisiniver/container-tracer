#!/bin/bash

# turn on bash's job control
set -m

echo "before running tracer"
# Start the primary process and put it in the background
./tracer &

echo "after running tracer"
# Start the helper process
./${PROC}

echo "after running proc"
# the my_helper_process might need to know how to wait on the
# primary process to start before it does its work and returns
  
  
# now we bring the primary process back into the foreground
# and leave it there
fg %1
