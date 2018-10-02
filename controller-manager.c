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
	fprintf(stderr, "%s\n", err);
	exit(1);
}

const char *firstAP () {
	FILE *fp;
	static char ap[255];

	fp = fopen (DB_FILE, "r");
	if (fp == NULL) DieWithError ("Failed opening DB file!");
	fscanf (fp, "%s", ap);
	fclose (fp);
	return ap;
}

const char *lastAP () {
	FILE *fp;
	static char ap[255];

	fp = fopen (DB_FILE, "r");
	if (fp == NULL) DieWithError ("Failed opening DB file!");

	while (!feof(fp)) {
		fscanf (fp, "%s", ap);
	}
	fclose (fp);
	return ap;
}

char *getClient (const char *apfile, int idclient) {
	FILE *fp;
	static char client[255];
	int x = 1, found = 0;

	fp = fopen (apfile, "r");
	if (fp == NULL) DieWithError ("Failed opening AP file!");

	while (!feof(fp)) {
		fscanf (fp, "%s", client);
		if (feof (fp)) break;
		if (x == idclient) found = 1;
		if (x == idclient) break;
		x++;
	}
	fclose (fp);

	if (found == 0) return NULL;

	return client;
}

int moveClientToPending (char apfile[255], char client[255]) {
	FILE *fp1, *fp2;
	static char clientmp[255], syscom[255];

	fp1 = fopen (apfile, "r");
	fp2 = fopen (".tmpfile", "w");

	if (fp1 == NULL) DieWithError ("Failed opening AP file!");
	
	// Removing client from AP file
	while (!feof(fp1)) {
		fscanf (fp1, "%s", clientmp);
		if ((strcmp (clientmp, client) != 0) && (!feof(fp1))) {
			fprintf (fp2, "%s\n", clientmp);
		}
	}
	fclose (fp1);
	fclose (fp2);

	strcpy (syscom, "mv .tmpfile ");
	strcat (syscom, apfile);
	system (syscom);
	system ("rm -f .tmpfile");

	// Appending client to PENDING list
	fp1 = fopen (PENDING_FILE, "a");
	if (fp1 == NULL) DieWithError ("Failed opening PENDING_FILE!");
	fprintf (fp1, "%s\n", client);
	fclose (fp1);

	return 0;
}

// Lists all clients in specific AP
void list_clients (const char *apfile) {
	FILE *fp;
	static char client[255];
	int x = 1;

	fp = fopen (apfile, "r");
	if (fp == NULL) DieWithError ("Failed opening AP file!");

	while (!feof(fp)) {
		fscanf (fp, "%s", client);
		if (feof (fp)) break;
		printf ("\t%d- %s\n", x, client);
		x++;
	}
	fclose (fp);
}

char *getAP (int idap) {
	FILE *fp;
	static char ap[255];
	int x = 1, found = 0;

	fp = fopen (DB_FILE, "r");
	if (fp == NULL) DieWithError ("Failed opening DB file!");

	while (!feof(fp)) {
		fscanf (fp, "%s", ap);
		if (feof (fp)) break;
		if (x == idap) found = 1;
		if (x == idap) break;
		x++;
	}
	fclose (fp);

	if (found == 0) return NULL;

	return ap;
}

static char *show_status () {
	FILE *fp;
	static char ap[255], client[255];
	static char change[10];
	int x = 1;

	fp = fopen (DB_FILE, "r");
	if (fp == NULL) DieWithError ("Failed opening DB file!");

	while (!feof(fp)) {
		fscanf (fp, "%s", ap);
		if (feof (fp)) break;
		printf ("%d- %s\n", x, ap);
		list_clients (ap);
		x++;
	}
	fclose (fp);

	fp = fopen (PENDING_FILE, "r");
	if (fp == NULL) DieWithError ("Failed opening PENDING file!");

	x = 1;
	printf ("\nPending:\n");

	while (!feof(fp)) {
		fscanf (fp, "%s", client);
		if (feof (fp)) break;
		printf ("\t%d- %s\n", x, client);
		x++;
	}
	fclose (fp);

	printf ("\nPor favor escolha qual cliente voce gostaria de mover para qual AP:\n");
	printf ("Formato: <AP_SRC> <CLIENT> <AP_DST>\n");
	printf ("(Ou digite 'q' para sair ou 'a' para atualizar a lista.\n");
	fgets (change, 10, stdin);

	return change;
}

int main(int argc, char *argv[]) { 
	int sock;
	struct sockaddr_in echoServAddr;
	struct sockaddr_in fromAddr;
	int structLen;
	int respStringLen;
	static char client[255], ap[255];
	char *action, *action_divided;
	char idapsrc_str[3], idclient_str[3], idapdst_str[3];
	char *apsrc_name, *apdst_name, *clientip;
	char apsrc[255], apdst[255], clientname[255];
	int idapsrc, idclient, idapdst;

	PktAction msg;
	
	// Vamos mostrar uma lista dos APs e seus clientes
	// e perguntar pro usuario qual cliente ele gostaria de mover para qual AP.
	// A funcao deve retornar uma string no formato "<CLIENTE> <AP>".

	system ("clear");
	
	while (1) {
		action = show_status ();

		while (action[0] == 'a') {
			system ("clear");
			action = show_status ();
		}

		if (action[0] == 'q') {
			printf ("Até!\n");
			exit (0);
		}

		// Agora precisamos separar a string retornada em 3: ID do AP SRC, ID do cliente e ID do AP DST
		action_divided = strtok (action, " ");
		strcpy (idapsrc_str, action_divided);

		action_divided = strtok (NULL, " ");
		strcpy (idclient_str, action_divided);

		action_divided = strtok (NULL, " ");
		strcpy (idapdst_str, action_divided);

		idapsrc = atoi (idapsrc_str);
		idclient = atoi (idclient_str);
		idapdst = atoi (idapdst_str);

		if (idapsrc == idapdst) DieWithError ("APs SRC and DST are the same!");

		apsrc_name = getAP (idapsrc);
		if (apsrc_name == NULL) DieWithError ("AP SRC not found!");
		strcpy (apsrc, apsrc_name);

		clientip = getClient (apsrc, idclient);
		if (clientip == NULL) DieWithError ("Client not found!");
		strcpy (clientname, clientip);

		apdst_name = getAP (idapdst);
		if (apdst_name == NULL) DieWithError ("AP DST not found!");
		strcpy (apdst, apdst_name);

	//	printf ("AP SRC: '%s'\n", apsrc);
	//	printf ("Client: '%s'\n", clientname);
	//	printf ("AP DST: '%s'\n", apdst);

		moveClientToPending (apsrc, clientname);

		strcpy (msg.aptogo, apdst);

		if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
			DieWithError("socket() falhou\n");

		memset(&echoServAddr, 0, sizeof(echoServAddr));
		echoServAddr.sin_family = AF_INET;
		echoServAddr.sin_addr.s_addr = inet_addr(clientname);
		echoServAddr.sin_port   = htons(STATION_MANAGER_PORT);

		if (inet_aton(clientname, &echoServAddr.sin_addr) == 0) {
			fprintf(stderr, "Falha ao conectar com cliente '%s' :(\n", clientname);
			exit(1);
		}
		printf ("Pedindo pro cliente '%s' ir para o AP '%s'...\n", clientname, apdst);
		sleep (4);

		sendto(sock, &msg, sizeof(msg), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr));

		system ("clear");

		close(sock);
	}
}
