#define CONTROLLER_DATABASE_PORT 5501
#define STATION_MANAGER_PORT 5502

#define APATUAL "apatual.txt"
#define APNEW "apnew.txt"
#define DB_FILE "DB.txt"
#define PENDING_FILE "PENDING.txt"

typedef struct __attribute__((__packed__)) pktact {
	char aptogo[255];
} PktAction;

typedef struct __attribute__((__packed__)) pktnot {
    char myip[255];
    char myap[255];
} PktNotifier;
