#!/bin/bash

IP=$1
TROCAS=$2
ESPERA=$3
VEL=$4

for i in `seq 1 $TROCAS`;
do

  echo "Enviando UDP pra $IP..."
  ../controller-manager $IP $VEL $i

  sleep $ESPERA

done
