#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "common.h"

// Comandos uteis:
	//1- Tentando associar 'manualmente' a um AP:
	// iw dev wlan0 disconnect; ifconfig wlan0 down; ifconfig wlan0 up; route add default gw 10.5.5.1 dev wlan0; iw dev wlan0 connect Escritorio ");

	//2- Salvar o canal dele no arquivo apatual
	// iwconfig wlan0 | grep Frequency | awk '{print $2}' | cut -c 11-15 | sed 's/\\.//g'

void DieWithError(char *err) {
	fprintf(stderr, "%s\n", err);
	exit(1);
}

void salvaAPAtual () {
	char cmd[255];

	strcpy (cmd, "iwconfig wlan0 | grep Point | awk '{print $6}' > ");
	strcat (cmd, APATUAL);
	system (cmd);

	printf ("Salvando ap atual: |%s|\n", cmd);

}

// 1- Checar se a interface associou a algum AP
// 2- Se tiver associado, retornar 0
int monitoraInterface () {
	char status[200];
	char comando[200];
	char canalatual[10];
	FILE *fp, *flog;

	system ("iwconfig wlan0 | grep Point | awk '{print $6}' > .status");
	fp = fopen (".status", "r");
	fscanf (fp, "%s", status);
	fclose (fp);

	flog = fopen("log-station.txt", "a");

	printf ("Status: |%s|\n", status);

	// Se nao associou a um AP
	if (strcmp (status, "dBm") == 0) {
		fprintf(flog, "\nNAO associou: %s\n", status);
		printf ("Nao consegui me associar a um AP =(\n");
		fclose (flog);
		return 1;
	}
	else {
		fprintf(flog, "\nAssociou: %s\n", status);
		printf ("Associado a '%s'.\n", status);
		fclose (flog);
		return 0;		
	}
}

void mudaAP (char *bssid) {
	char comando[200];
	char *data;
	int status;
	FILE *fp;

	// Salvando AP novo
	fp = fopen (APNEW, "w");
	if (fp == NULL) DieWithError ("Failed opening AP file!");
	fprintf (fp, "%s", bssid);
	fclose (fp);

	strcpy(comando, "iw dev wlan0 disconnect; iw dev wlan0 connect Escritorio ");
	strcat(comando, bssid);
//	system (comando);
	printf ("comando: |%s|\n", comando);

	printf ("Esperando associar ao novo AP...\n");
	sleep (2);
}

int main(int argc, char *argv[]) {
	int sock, conectado, apdavez;
	struct sockaddr_in echoServAddr;
	struct sockaddr_in echoClntAddr;
	unsigned int cliAddrLen;

	PktAction msg;

	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("socket() failed");

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = 0; /* Any incoming interface */
	echoServAddr.sin_port = htons(STATION_MANAGER_PORT);

	if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
		DieWithError("bind() failed");

	cliAddrLen = sizeof(echoClntAddr);

	for (;;) {
		printf("Station Manager: esperando comando do controller...");
		fflush(stdout);

		if ((recvfrom(sock, &msg, sizeof(msg), 0, (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
			DieWithError("recvfrom() falhou!\n");

		printf("\nRecebido comando de %s!\n", inet_ntoa(echoClntAddr.sin_addr));
		printf("aptogo: %s\n", msg.aptogo);

		salvaAPAtual ();

		conectado = 0;
		mudaAP (msg.aptogo);
		apdavez = 1;
		conectado = monitoraInterface();

		while (conectado != 0) {
			if (apdavez == 1) {
				mudaAP (APATUAL);
				apdavez = 0;
			}
			else {
				mudaAP (msg.aptogo);
				apdavez = 1;
			}
			conectado = monitoraInterface();
		}

		salvaAPAtual ();
	}
	close(sock);

	return 0;
}

