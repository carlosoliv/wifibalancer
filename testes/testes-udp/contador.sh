#!/bin/bash

for i in `seq 1 40`;
do

echo "$i $(grep "NAO" log-$i | wc -l )"

done



