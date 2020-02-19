#!/bin/sh

# source
# https://askubuntu.com/q/54747

# $1 is process name
# $2 is memory limit in MB

if [ $# -ne 2 ];
then
    echo "Invalid number of arguments"
    exit 0
fi

while true;
do
    pgrep "$1" | while read -r procId;
    do
        SIZE=$(pmap $procId | grep total | grep -o "[0-9]*")
        SIZE=${SIZE%%K*}
        SIZEMB=$((SIZE/1024))
        # echo "Process id = $procId Size = $SIZEMB MB"
        if [ $SIZEMB -gt $2 ]; then
            printf "SIZE has exceeded.\nKilling the process......"
            kill -9 "$procId"
            echo "Killed the process"
            exit 0
        # else
            # echo "SIZE has not yet exceeding"
        fi
    done

    sleep 1
done
