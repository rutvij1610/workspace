#include "headers.h"

uint8_t I_AM_SERVER_HELLO = 0x01;
uint8_t I_AM_CLIENT_HELLO = 0x02;

// Message type for client and server communication
uint8_t M_ID_LIST_FILES     = 0x02;
uint8_t M_ID_DOWNLOAD_FILES = 0x03;
uint8_t M_ID_UPLOAD_FILES   = 0x04;
uint8_t M_ID_SEARCH     	= 0x05;
uint8_t M_ID_DISCONNECT     = 0x06;
