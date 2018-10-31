// Handles all of the application layer
#ifndef APPLICATION_H
#define APPLICATION_H

#define CONTROL_PACKET_LEN 5
#define DATA_PACKET_LEN 4

#define MAX_DATA_LEN 25 // Max data length to be sent in on packet: 2^16.

#define DATA_C 1
#define START_C 2
#define END_C 3

#define FILESIZE_T 0
#define FILENAME_T 1

#define MAX_ATTEMPTS 3

/*
    Sends control packet to define the beggining or ending of file transmition

    Argument 'fd': serial port file descriptor
    Argument 'type': defines either if its the beggining or ending of file transmition (START_C | END_C)
    Argument 'filename': name of the file to be transmitted
    Argument 'length': size of the file to be transmitted
*/
int send_control_packet(int fd, int type, char * filename, unsigned int length);

/*
    Receives control packet containing information about the file to be received

    Argument 'fd': serial port file descriptor
    Argument 'type': determines either if its the beggining or ending of file transmition (START_C | END_C)
    Argument 'filename': name of the file to be received
    Argument 'file_length': size of the file to be received
*/
int receive_control_packet(int fd, int type, char * filename, unsigned int * file_length);

/*
    Sends data packet containing file content (partial or integral)

    Argument 'fd': serial port file descriptor
    Argument 'N': data packet sequence number
    Argument 'buffer': content to be transmitted
    Argument 'length': size of the content to be transmitted
*/
int send_data_packet(int fd, unsigned int N, char * buffer, unsigned int length);

/*
    Receives data packet containing file content (partial or integral)

    Argument 'fd': serial port file descriptor
    Argument 'buffer': buffer where content transmitted will be stored
    Argument 'length': size of the content received
*/
unsigned int receive_data_packet(int fd, char * buffer, int * buf_len);

/*
    Function responsible to send a certain file

    Argument 'port': serial port directory
    Argument 'filename': name of the file to be transmitted
    Argument 'file_content': buffer that contains content to be transmitted
    Argument 'length': file size
*/
int send_file(char * port, char * filename, char * file_content, int length);

/*
    Function responsible to receive a certain file

    Argument 'port': serial port directory
*/
int receive_file(char * port);

#endif
