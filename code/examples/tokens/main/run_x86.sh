#! /bin/bash

ROOT_PATH=`pwd`
LD_LIBRARY_PATH="$ROOT_PATH/../../bin/x86:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH

rm -f process1/process1.log 2> /dev/null
rm -f process2/process2.log 2> /dev/null
rm -f process3/process3.log 2> /dev/null

cd process1/x86
./process1 &
cd ../..
cd process2/x86
./process2 &
cd ../..
cd process3/x86
./process3
cd ../..

sleep 5

if grep -q "Token1: loop 10" process1/process1.log; then
    echo Info: all processes/token1 has been successfully executed
else
    echo Error: all processes/token1 has failed!!!
fi

if grep -q "Token2: loop 10" process1/process1.log; then
    echo Info: all processes/token2 has been successfully executed
else
    echo Error: all processes/token2 has failed!!!
fi
