#!/bin/bash

# Para solicitar a senha do sudo, que eh necessaria pra rodar o station-manager
[ "$UID" -eq 0 ] || exec sudo bash "$0" "$@"

controller=$1

# Caso o usuario aperte CTRL+C, finalizar corretamente
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
