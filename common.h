#define CONTROLLER_DATABASE_PORT 5501
#define STATION_MANAGER_PORT 5502

#define DB_FILE "DB.txt"
#define PENDING_FILE "PENDING.txt"

typedef struct __attribute__((__packed__)) pkt {
	char aptogo[255];
} PktAction;

typedef struct __attribute__((__packed__)) pkt {
    char myip[255];
    char myap[255];
} PktNotifier;
