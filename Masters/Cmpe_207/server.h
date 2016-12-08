#include "headers.h"

//============================================================
typedef struct ServerPeerInfoStruct {
	char* server_name;
	int socket;
	bool connected;
	FileInfoNode * front;
	FileInfoNode * rear;
	pthread_mutex_t server_peer_writer_queue_lock;
	pthread_cond_t server_peer_writer_queue_cv;
	bool get_files;
	// queue front
	// queue rear
	// queue lock
	// queue condition variable
} ServerPeerInfo;

//============================================================

FileInfo file_info_list[MAX_FILES];
uint32_t file_info_count = 0;
pthread_mutex_t file_info_list_lock;

//============================================================
ServerPeerInfo server_peer_info[MAX_SERVERS];
int connected_server_count = -1;
pthread_mutex_t server_peer_info_lock;

//============================================================

ServerInfo servers[MAX_SERVERS];
int total_server_count = 0;
char *config_file_name = "servers_config.txt";

//============================================================
