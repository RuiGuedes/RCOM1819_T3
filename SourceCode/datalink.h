// Handles all of the data link layer
#ifndef DATA_LINK_H
#define DATA_LINK_H

#define TRANSMITTER 0
#define RECEIVER    1

#define FLAG      0x7
#define ESC       0x7D
#define BST_BYTE  0x20

#define TRANS_A   0x03 // Transmitter commands and Receiver responses
#define REC_A     0x01 // Receiver commands and Transmitter responses

// List of Commands
#define SET_C     0x03
#define DISC_C    0x0B
#define DATA_C1   0x40
#define DATA_C0   0x00


// List of Responses
#define UA_C        0x07
#define RR_C0       0x05
#define RR_C1       0x85
#define REJ_C0      0x01
#define REJ_C1      0x81


// FRAME SEND AND RECEIVER FUNCTIONS
#define CTRL_FRAME_LEN   5     // Length of the control frame (in bytes)
#define DATA_FRAME_LEN   7     // Length of the data frame header and trailer (in bytes)
#define MAX_DATA_LEN     65535 // Max data length to be sent in on packet: 2^16.


// Global variables
extern int flag, attempts, DATA_C;


// DataLink API

/*
    Establish connection between serial ports

    Argument 'port': serial port directory
    Argument 'user': determines the user type: transmitter or receiver
*/
int llopen(char *port, int user);

/*
    Writes content to the serial port

    Argument 'fd': serial port file descriptor
    Argument 'buffer': buffer containing data to be sent
    Argument 'length': number of bytes to be sent
*/
int llwrite(int fd, char * buffer, int length);

/*
    Reads content from the serial port.

    Argument 'fd': serial port file descriptor
    Argument 'buffer': buffer where data will be stored
*/
int llread(int fd, char* buffer);

/*
    Terminates connection between serial ports

    Argument 'fd': serial port file descriptor
*/
int llclose(int fd);

// DataLink Aux Functions

/*
    Send Supervision or Unnumbered frames

    Argument 'fd': serial port file descriptor
    Argument 'addr_byte': address byte to be sent (TRANS_A | REC_A)
    Argument 'ctrl_byte': control byte to be sent (SET_C | UA_C)
*/
void send_control_frame(int fd, int addr_byte, int ctrl_byte);

/*
    Receive Supervision or Unnumbered frames

    Argument 'fd': serial port file descriptor
    Argument 'addr_byte':  address byte to be received (TRANS_A | REC_A)
*/
unsigned char receive_control_frame(int fd, int addr_byte);

/*
    Sends frame containing the data to be sent

    Argument 'fd': serial port file descriptor
    Argument 'buffer': buffer containing the data to be sent
    Argument 'length': number of bytes to be sent
*/
int send_data_frame(int fd, char * buffer, int length);

/*
    Receives frame containing the data that was sent

    Argument 'fd': serial port file descriptor
    Argument 'data_c': received control field (0 | 1)
    Argument 'data': where data will be stored
*/
int receive_data_frame(int fd, unsigned char * data_c, char * data);

#endif
