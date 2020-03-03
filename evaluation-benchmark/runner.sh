#!/bin/bash

EXE="bench.out"
g++ benchmark.cpp -O2 -o $EXE

limiter(){
    ../killIf.sh $EXE 5000
}

pmapper(){
    pid="$(pgrep $EXE)"
    echo $pid
    sudo pmap -x $pid
}

run(){
    ./$EXE
}

checker(){
    limiter & run
}

checker & run
