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

#define PORTA 8881

void DieWithError(char *err) {
	printf ("%s", err);
	exit(1);
}

typedef struct __attribute__((__packed__)) pkt {
	char aptogo[255];
} Pacote;

char * executeCmd (char * cmd) {
	FILE *fp;
	char *retorno;
	
	fp = fopen ("cmd.sh", "w");
	fprintf (fp, "#!/bin/bash\n");
	fprintf (fp, "%s > saida\n\n", cmd);
	fclose (fp);
	system ("/bin/bash cmd.sh");

	fp = fopen ("saida", "r");
	fscanf (fp, "%s", retorno);
	fclose (fp);
	if (!retorno) retorno = "";

//	printf ("Executando: %s\n", cmd);
//	printf ("Retorno: %s\n", retorno);

	return retorno;	
}

char * executeCmdFilho (char * cmd) {
	FILE *fp;
	char *retorno;

	fp = fopen ("cmdf.sh", "w");
	fprintf (fp, "#!/bin/bash\n");
	fprintf (fp, "%s > saidaf\n", cmd);
	fclose (fp);
	system ("/bin/bash cmdf.sh");

	fp = fopen ("saidaf", "r");
	fscanf (fp, "%s", retorno);
	fclose (fp);
	if (!retorno) retorno = "";

	printf ("Executando: %s\n", cmd);
	printf ("Retorno: %s\n", retorno);

	return retorno;	
}

// 1- Checar se a interface associou a algum AP
// 2- Se tiver associado, salvar numa variavel [global?] e terminar a funcao
// 3- Se não tiver associado, tenta se associar no BSSID anterior
// (devemos ter 2 variaveis globais: apatual e apanterior

void monitoraInterface () {
	char *data;
	char comando[200];
	char canalatual[10];
	FILE *fp, *flog;

	data = executeCmdFilho ("iwconfig wlan0 | grep Point | awk '{print $6}'");
	flog = fopen("log-server-filho.txt", "a");

	printf ("DATA: |%s|\n", data);

	// Se nao associou a um AP
	if ((data[0] == 'd') && (data[1] == 'B') && (data[2] == 'm')) {
		fprintf(flog, "\nNAO associou: %s\n", data);

		fp = fopen("apatual", "r");
		fscanf(fp, "%s", canalatual);
		fclose(fp);

		strcpy(comando, "iw dev wlan0 disconnect; ifconfig wlan0 down; ifconfig wlan0 up; route add default gw 10.5.5.1 dev wlan0; iw dev wlan0 connect Escritorio ");
		strcat(comando, canalatual);
		data = executeCmdFilho (comando);

		fprintf(flog, "Tentando conectar ao ultimo AP: %s\n", comando);
		fprintf(flog, "Resultado: %s\n\n", data);
		fclose (flog);

	}
	// Se associou a um AP, salvar o canal dele no arquivo apatual
	else {
		fprintf(flog, "Associado! %s\n", data);
		strcpy (comando, "iwconfig wlan0 | grep Frequency | awk '{print $2}' | cut -c 11-15 | sed 's/\\.//g'");
		data = executeCmdFilho (comando);

		fp = fopen("apatual", "w");
		fprintf(fp, "%s", data);
		fclose (flog);
		fclose (fp);
	}
}

void mudaCanal (char *bssid, int canal, int espera) {
	char comando[200];
	char *data;
	char canal_str[10];
	int status;

	sprintf (canal_str, "%d", canal);
	strcpy(comando, "iw dev wlan0 disconnect; iw dev wlan0 connect Escritorio ");
	strcat(comando, canal_str);

	data = executeCmd (comando);
	printf ("comando: |%s|\n", comando);
	if (data) printf ("resultado: |%s|\n", data);
}

// Essa funcao vai receber do pai a mensagem de atualizacao do AP
// A missao do filho é estar constantemente checando se a interface esta associada a algum AP

// 1- Esperar 2 seg e olhar o arquivo 'conectado'
// 2- Se esse arquivo existir, checar se a interface esta conectada e remove o arquivo
// 3- Se nao existir, checar se a interface esta conectada e voltar pro passo 1
void filho () {
	
	for (;;) {
		sleep (1);
//		monitoraInterface();
		system ("/bin/bash monitoraInterface.sh");
	}
}

void salva (int vel, int num) {
	FILE *fp;

	char velFile[255];
	strcpy (velFile, "log-server-");
	sprintf (velFile, "%s%d", velFile, vel);

	fp = fopen (velFile, "a");
	fprintf (fp, "%d\n", num);
	fclose (fp);
}

int main(int argc, char *argv[]) {
	int sock;
	struct sockaddr_in echoServAddr;
	struct sockaddr_in echoClntAddr;
	unsigned int cliAddrLen;

	Pacote msg;

//	int id = fork();

	// Sou o processo filho
//	if (id == 0) filho ();
	
	// I'm your father
//	else {

		if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
			DieWithError("socket() failed");

		memset(&echoServAddr, 0, sizeof(echoServAddr));
		echoServAddr.sin_family = AF_INET;
		echoServAddr.sin_addr.s_addr = 0; /* Any incoming interface */
		echoServAddr.sin_port = htons(PORTA);

		if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
			DieWithError("bind() failed");

		cliAddrLen = sizeof(echoClntAddr);

		for (;;) {
			printf("Esperando comando do servidor...");
			fflush(stdout);
	
			if ((recvfrom(sock, &msg, sizeof(msg), 0, (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
				DieWithError("recvfrom() falhou!\n");

			printf("\nRecebido comando de %s!\n", inet_ntoa(echoClntAddr.sin_addr));
			printf("vel: %d\n", msg.vel);
			printf("num: %d\n", msg.num);
		
			salva(msg.vel, msg.num);
		}
		close(sock);
//	}
	return 0;
}

