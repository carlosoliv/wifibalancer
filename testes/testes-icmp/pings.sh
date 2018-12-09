#!/bin/bash

IP=$1
TROCAS=$2
ESPERA=$3

for i in `seq 1 $TROCAS`;
do

  echo "Pingando $IP..."
  ping -q -c1 $IP > /dev/null
 
  if [ $? -eq 0 ]
  then
    echo "Respondeu"
    echo "Respondeu" >> log
  else
    echo "NAO respondeu"
    echo "NAO respondeu" >> log
  fi

  sleep $ESPERA

done
