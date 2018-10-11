#!/bin/bash
[ "$UID" -eq 0 ] || exec sudo bash "$0" "$@"

controller=$1

trap "kill_stuff" INT

function kill_stuff() {
    killall station-manager
    exit 0
}

if [ "$#" -ne 1 ]; then
    echo "Use: ./station.sh <controller_ip>"
    exit 1
fi

./station-manager &

while true
do
    ./station-notifier $controller
    sleep 5
done
