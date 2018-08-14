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

void DieWithError(char *err) {
	printf ("%s", err);
	exit(1);
}

typedef struct __attribute__((__packed__)) pkt {
	int vel;
	int num;
} Pacote;

void salva (int vel, int num) {
	FILE *fp;

	char velFile[255];
	strcpy (velFile, "log-client-");
	sprintf (velFile, "%s%d", velFile, vel);

	fp = fopen (velFile, "a");
	fprintf (fp, "%d\n", num);
	fclose (fp);
}

int main(int argc, char *argv[]) { 
	int sock;
	struct sockaddr_in echoServAddr;
	struct sockaddr_in fromAddr;

	char *servIP;
	int structLen;
	int respStringLen;

	if (argc != 4) {
		char str[100];
		strcat (str, "Uso: \n");
		strcat (str, argv[0]);
		strcat (str, " <ip> <vel> <num>\n");
		DieWithError (str);
	}

	Pacote msg;
	msg.vel = atoi(argv[2]);
	msg.num = atoi(argv[3]);

	salva (msg.vel, msg.num);

	servIP = argv[1];

	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("socket() failed\n");

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	        echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
	echoServAddr.sin_port   = htons(STATION_MANAGER_PORT);

	if (inet_aton(servIP , &echoServAddr.sin_addr) == 0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	int tempint = 0;

	tempint = sendto(sock, &msg, sizeof(msg), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)); 

	if (tempint == -1 ) {
		printf("Sent struct size: %d\n", tempint);
		DieWithError("sendto() sent a different number of bytes than expected\n");
	}

	close(sock);
	exit(0);
}
