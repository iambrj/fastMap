#!/bin/bash

EXE="bench.out"
LIMIT="8000"
g++ benchmark.cpp -O2 -o $EXE

limiter(){
    ./killIf.sh $EXE $LIMIT
}

run(){
    ./$EXE
}

checker(){
    run&
    limiter&
}

checker
killall "killIf.sh"
