#!/bin/bash

for i in `seq 1 50`;
do
	if [ -a log-$i ]; then
		echo "$i $(grep "NAO" log-$i | wc -l )"
	fi
done



