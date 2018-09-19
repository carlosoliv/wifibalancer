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

void DieWithError(char *err) {
	printf ("%s", err);
	exit(1);
}

// Checar se o IP esta na lista de pendentes
int checaPendencia (char myip[255]) {
	FILE *fp;
	char ipclient[255];
	int found = 0;

	fp = fopen (PENDING_FILE, "r");
	if (fp == NULL) DieWithError ("Failed opening PENDING file!");

	while (!feof(fp)) {
		fscanf (fp, "%s", ipclient);
		if (feof (fp)) break;
		if (strcmp (ipclient, myip) == 0) found = 1;
		if (strcmp (ipclient, myip) == 0) break;
	}
	fclose (fp);

	return found;
}

// Remover o AP da lista de pendentes e adicionar no final do arquivo AP
void removePendencia (char myip[255], char myap[255]) {
	FILE *fp1, *fp2;
	static char clientmp[255], syscom[255];

	fp1 = fopen (PENDING_FILE, "r");
	fp2 = fopen (".tmppendingfile", "w");

	if (fp1 == NULL) DieWithError ("Failed opening AP file!");
	
	// Removing client from PENDING file
	while (!feof(fp1)) {
		fscanf (fp1, "%s", clientmp);
		if ((strcmp (clientmp, client) != 0) && (!feof(fp1))) {
			fprintf (fp2, "%s\n", clientmp);
		}
	}
	fclose (fp1);
	fclose (fp2);
	strcpy (syscom, "mv .tmppendingfile ");
	strcat (syscom, PENDING_FILE);
	system (syscom);
	system ("rm -f .tmppendingfile");

	// Appending client to AP list
	fp1 = fopen (myap, "a");
	if (fp1 == NULL) DieWithError ("Failed opening AP file!");
	fprintf (fp1, "%s\n", myip);
	fclose (fp1);
}

void atualizaEstacao (char myip[255], char myap[255]) {
	// ok 1- Checar se o IP esta na lista de pendentes
	// 2- Se estiver, remover o AP da lista de pendentes e adicionar no final do arquivo AP
	// 3- Opcional: re-ordenar a lista de APs com os mais ocupados primeiro
	int  pendente = checaPendencia (myip);

	if (pendente == 1) {
		removePendencia (myip, myap);
	}
}

int main (int argc, char *argv[]) {
	int sock;
	struct sockaddr_in echoServAddr;
	struct sockaddr_in echoClntAddr;
	unsigned int cliAddrLen;

	PktNotifier msg;

	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("socket() falhou!");

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = 0;
	echoServAddr.sin_port = htons(CONTROLLER_DATABASE_PORT);

	if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
		DieWithError("bind() failed");

	cliAddrLen = sizeof(echoClntAddr);

	for (;;) {
		printf("Esperando comando do servidor...");
		fflush(stdout);

		if ((recvfrom(sock, &msg, sizeof(msg), 0, (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
			DieWithError("recvfrom() falhou!\n");

		printf("\nRecebida notificacao de %s!\n", inet_ntoa(echoClntAddr.sin_addr));
		printf("myip: %s\n", msg.myip);
		printf("myap: %s\n", msg.myap);
	
		atualizaEstacao (msg.myip, msg.myap);
	}
	close(sock);

	return 0;
}

