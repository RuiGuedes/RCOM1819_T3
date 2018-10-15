// Handles all of the data link layer
#ifndef DATA_LINK_H
#define DATA_LINK_H

#define TRANSMITTER 0
#define RECEIVER    1

#define FLAG      0x7

#define TRANS_A   0x03 // Transmitter commands and Receiver responses
#define REC_A     0x01 // Receiver commands and Transmitter responses

// List of Commands
#define SET_C     0x03
#define DATA_C1   0x40
#define DATA_C0   0x00


// List of Responses
#define UA_C        0x07
#define RR_C0       0x05
#define RR_C1       0x85
#define REJ_C0      0x01
#define REJ_C1      0x81


// List of returns
#define SUCCESS   1
#define INSUCCESS 0


// FRAME SEND AND RECEIVER FUNCTIONS

#define CTRL_FRAME_LEN   5 // Length of the control frame (in bytes)
#define DATA_FRAME_LEN   6 // Length of the data frame (in bytes)

// Global variables

extern int flag, attempts, DATA_C;

/*

*/
int llopen(int port, int user);


/* Send Supervision or Unnumbered frames

  Argument 'fd': File descriptor of the serial port
  Argument 'addr_byte': Address byte to be sent (TRANS_A | REC_A)
  Argument 'ctrl_byte': Control byte to be sent (SET_C | UA_C)
*/
void send_control_frame(int fd, int addr_byte, int ctrl_byte);

/* Send Supervision or Unnumbered frames

  Argument 'fd': File descriptor of the serial port
  Argument 'addr_byte': Address byte expected to be received (TRANS_A | REC_A)
  Argument 'ctrl_byte': Control byte expected to be received (SET_C | UA_C)

  TODO argument to define timeout time
*/
int receive_control_frame(int fd, int addr_byte, int ctrl_byte);

/*

*/
int llwrite(int fd, char * buffer, int length);

/*

*/
int send_data_frame(int fd, char * buffer, int length);

int llread(int fd, char* buffer);

int receive_data_frame(int fd, int addr_byte, int ctrl_byte);

#endif
