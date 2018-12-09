#!/bin/bash

for i in {1..50..5};
do
	if [ -a log-client-$i ]; then
		V1=$(wc -l log-client-$i | awk '{print $1}')
		V2=$(wc -l log-server-$i | awk '{print $1}')
		echo "$i $(expr $V1 - $V2)"
	fi
done



