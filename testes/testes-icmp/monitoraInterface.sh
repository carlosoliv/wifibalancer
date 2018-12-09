#!/bin/bash
LOGF="log-server-filho.txt"
ASSO=$(iwconfig wlan0 | grep Point | awk '{print $6}')
APDEFAULT="2462"
MUDOU="0"

if [ "$ASSO" == "dBm" ] ; then
#	echo "Nao associado"
	echo "Nao associado" >> $LOGF
	date >> $LOGF

	if [ -f "apatual" ];
	then
		iw dev wlan0 disconnect
		ifconfig wlan0 down
		ifconfig wlan0 up
		route add default gw 10.5.5.1 dev wlan0
	
		APATUAL=$(cat apatual)
		iw dev wlan0 connect Escritorio $APATUAL
		echo "Conectando em $APATUAL"
		echo "Tentando conectar ao ultimo AP: $APATUAL" >> $LOGF
		MUDOU="1"
	else
		echo "" >> $LOGF
		echo "Arquivo apatual nao existe!" >> $LOGF
		echo "Tentando se conectar no AP Default: $APDEFAULT" >> $LOGF
		echo $APDEFAULT > apatual

	fi
	echo "" >> $LOGF
else
#	echo "Associado"
	if [ "$MUDOU" == "1" ] ; then
		echo "Associado" >> $LOGF
		MUDOU="0"
	fi

	iwconfig wlan0 | grep Frequency | awk '{print $2}' | cut -c 11-15 | sed 's/\.//g' > apatual

fi


