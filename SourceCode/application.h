// Handles all of the application layer
#ifndef APPLICATION_H
#define APPLICATION_H

#define CONTROL_PACKET_LEN 5
#define DATA_PACKET_LEN 4

#define MAX_DATA_LEN 65535 // 2^16

#define DATA_C 1
#define START_C 2
#define END_C 3

#define FILESIZE_T 0
#define FILENAME_T 1

#define MAX_ATTEMPTS 3


int send_control_packet(int fd, int type, char * filename, unsigned int length);

int receive_control_packet(int fd, int type, char * filename, unsigned int * file_length);

int send_data_packet(int fd, int N, char * buffer, unsigned int length);

int receive_data_packet(int fd, char * buffer, int * buf_len);

#endif
