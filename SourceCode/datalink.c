#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "datalink.h"
#include "serialconfig.h"
#include <stdio.h>

// Init global variables

int userType;
int flag = 1;
int attempts = 1;
int DATA_C = DATA_C1;
unsigned char data[255];

/*
  Manages alarm interruptions
*/
void manage_alarm() {
  flag=1;
  attempts++;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////// Connection Establishment /////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int llopen(char *port, int user) {
  userType = user;
  int fd = init_serial_n_canon(port);

  switch(userType) {
    case TRANSMITTER:
      //Manage alarm interruptions
      (void) signal(SIGALRM, manage_alarm);

      while (attempts < 4) {
        // Send SET command
        send_control_frame(fd, TRANS_A, SET_C);
        printf("SET command sent\n");

        // Set alarm for 3 seconds
        if(flag){
          alarm(3);
          flag=0;
        }

        // Setup receiving UA message
        if (receive_control_frame(fd, REC_A) ==  UA_C) {
          printf("UA Command received\n");
          break;
        }
        else {
          printf("UA Command not received. Attempting to reconnect.\n");
        }
      }

      if (attempts >= 4)
        printf("UA Command not received\n");
      break;
    case RECEIVER:
      // Setup receiving SET message
      while(receive_control_frame(fd, TRANS_A) != SET_C);
      printf("SET Command received\n");

      // Send UA response
      send_control_frame(fd, REC_A, UA_C);
      printf("UA Command sent\n");
    break;
    default:
      return -1;
    break;
  }
  return fd;
}

void send_control_frame(int fd, int addr_byte, int ctrl_byte) {
  unsigned char frame[CTRL_FRAME_LEN];

  frame[0] = FLAG;
  frame[1] = addr_byte;
  frame[2] = ctrl_byte;
  frame[3] = frame[1]^frame[2];
  frame[4] = FLAG;

  write_serial(fd, frame, CTRL_FRAME_LEN);
}

unsigned char receive_control_frame(int fd, int addr_byte) {
  unsigned char byte, ctrl_byte;

  enum set_states {START, FLAG_REC, A_REC, C_REC, BCC_OK, END};
  enum set_states state = START;

  while (state != END) {
    if (flag && (userType == TRANSMITTER)) {
      return 0;
    }

    read_serial(fd, &byte, 1);

    switch(state) {
      case START:
      if(byte == FLAG) {
        state = FLAG_REC;
      }
      break;
      case FLAG_REC:
      if(byte == addr_byte) {
        state = A_REC;
      }
      else if(byte != FLAG) {
        state = START;
      }
      break;
      case A_REC:
      if((byte == SET_C) || (byte == DISC_C) || (byte == UA_C) || (byte == RR_C0) || (byte == RR_C1) || (byte == REJ_C0) || (byte == REJ_C1)) {
        ctrl_byte = byte;
        state = C_REC;
      }
      else if (byte == FLAG) {
        state = FLAG_REC;
      }
      else {
        state = START;
      }
      break;
      case C_REC:
      if(byte == (addr_byte^ctrl_byte)) {
        state = BCC_OK;
      }
      else if (byte == FLAG) {
        state = FLAG_REC;
      }
      else {
        state = START;
      }
      break;
      case BCC_OK:
      if(byte == FLAG){
        state = END;
      }
      else {
        state = START;
      }
      break;
      case END:
      break;
    }
  }

  return ctrl_byte;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////// Data Transmission - Transmitter //////////////////////
////////////////////////////////////////////////////////////////////////////////

int llwrite(int fd, char * buffer, int length) {
  // Local variables
  int num_written_bytes = -1;

  // Reset global variables
  attempts = 1;
  flag = 1;

  while(attempts < 4) {
    // Send data frame
    num_written_bytes = send_data_frame(fd, buffer, length);

    // Set alarm for 3 seconds
    if(flag){
      alarm(3);
      flag = 0;
    }

    //Check for receiver response
    unsigned char command = receive_control_frame(fd, REC_A);

    switch(command) {
      case RR_C0:
        DATA_C = DATA_C0;
        printf("Receiver ready. Data transmitted.\n");

        return num_written_bytes;
      break;
      case RR_C1:
        DATA_C = DATA_C1;
        printf("Receiver ready. Data transmitted.\n");

        return num_written_bytes;
      break;
      case REJ_C0:
      case REJ_C1:
        printf("Receiver ready. Data transmitted.\n");
      break;
      default:
        printf("Command not received. Attempting to retransmit data.\n");
      break;
    }
  }
  return -1;
}

int send_data_frame(int fd, char * buffer, int length) {
  unsigned char frame[DATA_FRAME_LEN + length], bbc2 = 0;
  DATA_C = DATA_C == 0 ? DATA_C1 : DATA_C0;

  frame[0] = FLAG;
  frame[1] = TRANS_A;
  frame[2] = DATA_C;
  frame[3] = frame[1]^frame[2];

  for(int i = 0; i < length; i++) {
    frame[4+i] = buffer[i];
    bbc2 ^= buffer[i];
  }

  frame[4+length] = bbc2;
  frame[5+length] = FLAG;

  return write_serial(fd, frame, DATA_FRAME_LEN + length);
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////// Data Transmission - Receiver /////////////////////////
////////////////////////////////////////////////////////////////////////////////


int llread(int fd, unsigned char* buffer) {
  // Reset DATA_C variable
  DATA_C = DATA_C == 0 ? DATA_C1 : DATA_C0;

  int len = receive_data_frame(fd);

  buffer = data;

  printf("Message read %s\n", buffer);

  return len;
}

// TODO: Fix details on this function (The return value must be the length of the data)
// TODO: DATA_C holds the number of the frame the receiver expects to read. To ensure the function is readable, only do the necessary actions after processing the packet (outside the loop)
int receive_data_frame(int fd) {
  unsigned int index = 0;
  unsigned char byte, ctrl_byte, bbc2 = 0;

  enum set_states {START, FLAG_REC, A_REC, C_REC, BCC_OK, END};
  enum set_states state = START;

  while (state != END) {
    read_serial(fd, &byte, 1);

    switch(state) {
      case START:
      if(byte == FLAG) {
        state = FLAG_REC;
      }
      break;
      case FLAG_REC:
      if(byte == TRANS_A) {
        state = A_REC;
      }
      else if (byte != FLAG) {
        state = START;
      }
      break;
      case A_REC:
      if((DATA_C == DATA_C0 && byte == DATA_C0) || (DATA_C == DATA_C1 && byte == DATA_C1)) {
        ctrl_byte = byte;
        DATA_C = DATA_C == DATA_C0 ? DATA_C1 : DATA_C0;
        state = C_REC;
      }
      else if((DATA_C == DATA_C1 && byte == DATA_C0) || (DATA_C == DATA_C0 && byte == DATA_C1)) {
        send_control_frame(fd, REC_A, byte == DATA_C0 ? RR_C0 : RR_C1);
        state = END;
      }
      else if(byte == FLAG){
        state = FLAG_REC;
      }
      else {
        state = START;
      }
      break;
      case C_REC:
      if(byte == (TRANS_A^ctrl_byte)) {
        state = BCC_OK;
      }
      else if(byte == FLAG) {
        state = FLAG_REC;
      }
      else {
        state = START;
      }
      break;
      case BCC_OK:
      if(byte == FLAG) {
        state = END;
      }
      else {
        bbc2 ^= byte;
        data[index++] = byte;
      }
      break;
      case END:
      break;
    }
  }

  // Check BBC2 status
  if(bbc2 == 0) {
    send_control_frame(fd, REC_A, DATA_C == DATA_C0 ? RR_C1 : RR_C0);
    DATA_C = DATA_C == DATA_C0 ? DATA_C1 : DATA_C0;
  }
  else {
    send_control_frame(fd, REC_A, DATA_C == DATA_C0 ? REJ_C1 : REJ_C0);
    return -1;
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////// Termination of connection ////////////////////////////
////////////////////////////////////////////////////////////////////////////////
