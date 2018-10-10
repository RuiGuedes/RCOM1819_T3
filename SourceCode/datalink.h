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

// List of Responses
#define UA_C      0x07

// List of Responses
#define SUCCESS   1
#define INSUCCESS 0


// FRAME SEND AND RECEIVER FUNCTIONS

#define CTRL_FRAME_LEN   5 // Length of the control frame (in bytes)

// Global variables

extern int flag, attempts;

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

#endif
