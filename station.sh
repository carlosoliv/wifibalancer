#!/bin/bash
controller=$1

if [ "$#" -ne 1 ]; then
    echo "Use: ./station.sh <controller_ip>"
    exit 1
fi

sudo ./station-manager &

while true
do
    ./station-notifier $controller
    sleep 5
done
