#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define STATION_MANAGER_PORT 5502
#define DB_FILE "DB.txt"
#define PENDING_FILE "PENDING.txt"

typedef struct __attribute__((__packed__)) pkt {
	char aptogo[255];
} Pacote;

void DieWithError(char *err) {
	fprintf(stderr, "%s\n", err);
	exit(1);
}

const char *firstAP (char *dbfile) {
	FILE *fp;
	static char ap[255];

	fp = fopen (dbfile, "r");
	if (fp == NULL) DieWithError ("Failed opening DB file!");
	fscanf (fp, "%s", ap);
	fclose (fp);
	return ap;
}

const char *lastAP (char *dbfile) {
	FILE *fp;
	static char ap[255], ap2[255];

	fp = fopen (dbfile, "r");
	if (fp == NULL) DieWithError ("Failed opening DB file!");

	while (!feof(fp)) {
		fscanf (fp, "%s", ap);
		fscanf (fp, "%s", ap2);
	}
	fclose (fp);
	return ap;
}

const char *getClient (const char *apfile) {
	FILE *fp;
	static char client[255];

	fp = fopen (apfile, "r");
	if (fp == NULL) DieWithError ("Failed opening AP file!");
	fscanf (fp, "%s", client);
	fclose (fp);
	return client;
}

int moveClientToPending (const char *apfile, const char *client, char *pending_file) {
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

	// Appending client to PENDING list
	fp1 = fopen (pending_file, "a");
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
		printf ("\t%d- %s\n", x, client);
		x++;
	}
	fclose (fp);
}

const char *show_status (const char *dbfile) {
	FILE *fp;
	static char ap[255], client[255];
	static char change[10];
	int x = 1;

	system ("clear");

	fp = fopen (dbfile, "r");
	if (fp == NULL) DieWithError ("Failed opening DB file!");

	while (!feof(fp)) {
		fscanf (fp, "%s", ap);
		printf ("%d- %s\n", x, ap);
		x++;

		list_clients (ap);
	}
	fclose (fp);

	fp = fopen (PENDING_FILE, "r");
	if (fp == NULL) DieWithError ("Failed opening PENDING file!");

	x = 1;
	printf ("Pending:\n");

	while (!feof(fp)) {
		fscanf (fp, "%s", client);
		printf ("\t%d- %s\n", x, client);
		x++;
	}
	fclose (fp);

	printf ("Por favor escolha qual cliente voce gostaria de mover para qual AP:\n");
	printf ("Exemplo: para mover o cliente 2 para o AP 3, digite \"2 3\"");
	printf ("\n(Ou digite 'x' para sair ou 'a' para atualizar a lista.\n");
	scanf ("%s", change);
	
	return change;

}

int main(int argc, char *argv[]) { 
	int sock;
	struct sockaddr_in echoServAddr;
	struct sockaddr_in fromAddr;
	int structLen;
	int respStringLen;
	static char client[255], ap[255];

	Pacote msg;
	
	// Vamos mostrar uma lista dos APs e seus clientes
	// e perguntar pro usuario qual cliente ele gostaria de mover para qual AP.
	// A funcao deve retornar uma string no formato "<CLIENTE> <AP>".

	const char *action = show_status (DB_FILE);

	while (strcmp (action, "a") == 0)
		action = show_status (DB_FILE);
	
	if (strcmp (action, "x") == 0) {
		printf ("Até!\n");
		exit (0);
	}

	// Agora precisamos separar a string retornada em 2: ID do cliente e ID do AP

	//client = str

/*
	const char *firstap = firstAP(DB_FILE);
	const char *lastap = lastAP(DB_FILE);
	const char *firstclient = getClient(firstap);

	if ((strcmp (firstap, "") == 0) || ((strcmp (lastap, "") == 0)))
		DieWithError ("Cant find APs!");
	if (strcmp (firstclient, "") == 0) DieWithError ("Cant find clients!");

	moveClientToPending (firstap, firstclient, PENDING_FILE);

	strcpy (msg.aptogo, lastap);

	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("socket() failed\n");

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = inet_addr(firstclient);
	echoServAddr.sin_port   = htons(STATION_MANAGER_PORT);

	if (inet_aton(firstclient , &echoServAddr.sin_addr) == 0) {
		fprintf(stderr, "Failed connecting to client '%s' :(\n", firstclient);
		exit(1);
	}

	printf ("Asking client '%s' to move to AP '%s'...\n", firstclient, lastap);

	sendto(sock, &msg, sizeof(msg), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr));
	close(sock);

	exit(0);
	*/
}
