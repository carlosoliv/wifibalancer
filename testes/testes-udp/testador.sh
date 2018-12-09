#!/bin/bash

# Script para repetir o trocador com variacoes de velocidade de trafego de rede
# Etapas:
# 1- Iniciar iperf3 com velocidade i
# 2- Esperar 2 seg
# 3- Iniciar trocador
# 4- Mover arquivo de log para log-i
# 5- Terminar o iperf e voltar pro passo 1 com velocidade i+1

IP="192.168.0.101"
VELMAX="50"
NUMTROCAS="1000"
ESPERA="0"

rm -rf log-*

for i in {1..50..5};
do

  echo "Iniciando iperf3 com velocidade $i Mbits/s..."
  echo iperf3 -c $IP -u -t 0 -b$i\m
  iperf3 -c $IP -u -t 0 -b$i\m > /dev/null 2>&1 &

  sleep 2

  echo "Iniciando cliente para $NUMTROCAS testes, com $ESPERA seg entre cada envio..."
  ./udp.sh $IP $NUMTROCAS $ESPERA $i

  echo "Finalizando iperf3..."
  killall iperf3
  sleep 1
  ping -c 1 -w 99999999999999999999 $IP

  echo ""
done
