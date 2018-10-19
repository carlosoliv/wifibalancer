#!/bin/bash

# Caso o usuario aperte CTRL+C, finalizar corretamente
trap "kill_stuff" INT

function kill_stuff() {
    killall controller-database
    exit 0
}

(./controller-database > controller.log 2> /dev/null &)

./controller-manager
