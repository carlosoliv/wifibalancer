#!/bin/bash

trap "kill_stuff" INT

function kill_stuff() {
    killall controller-database
    exit 0
}

(./controller-database > controller.log 2> /dev/null &)

./controller-manager
