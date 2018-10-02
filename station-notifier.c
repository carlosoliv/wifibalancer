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

int main(int argc, char *argv[]) { 
	int sock;
	struct sockaddr_in echoServAddr;
	struct sockaddr_in fromAddr;

	char servIP[255];
	char myap[255], myip[255], cmd[255], mynet[255];
	int structLen;
	int respStringLen;
	FILE *fp;

	if (argc != 2) {
		char str[100];
		strcat (str, "Uso: \n");
		strcat (str, argv[0]);
		strcat (str, " <ip_controller>\n");
		DieWithError (str);
	}
	strcpy (servIP, argv[1]);

	// Pegando a interface de rede:
	system ("rm -rf .tmpfile");
	system ("iwconfig 2>&1 |grep IEEE | awk '{print $1}' > .tmpfile");
	fp = fopen (".tmpfile", "r");
	fscanf (fp, "%s", mynet);
	fclose (fp);
	system ("rm -rf .tmpfile");

//	printf ("Interface detectada: '%s'\n", mynet);

	// Pegando o AP:
	system ("rm -rf .myap");
	strcpy (cmd, "iwconfig ");
	strcat (cmd, mynet);
	strcat (cmd, " | grep Point | awk '{print $6}' > .myap");
//	printf ("cmd: '%s'\n", cmd);
	system (cmd);
	fp = fopen (".myap", "r");
	fscanf (fp, "%s", myap);
	fclose (fp);
	system ("rm -rf .myap");

	// Pegando o IP:
	system ("rm -rf .myip");
	strcpy (cmd, "ifconfig ");
	strcat (cmd, mynet);
	strcat (cmd, " | grep 'inet addr' | awk '{print $2}' | sed 's/addr://g' > .myip");
//	printf ("cmd: '%s'\n", cmd);
	system (cmd);
	fp = fopen (".myip", "r");
	fscanf (fp, "%s", myip);
	fclose (fp);
	system ("rm -rf .myip");

	PktNotifier msg;
	strcpy (msg.myap, myap);
	strcpy (msg.myip, myip);

	printf ("AP: %s\nIP: %s\n", myap, myip);
	printf ("Notificando controlador '%s'...\n", servIP);

	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("socket() failed\n");

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	        echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
	echoServAddr.sin_port   = htons(CONTROLLER_DATABASE_PORT);

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
