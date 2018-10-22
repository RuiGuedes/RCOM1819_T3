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
int DATA_C = DATA_C0;

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
      signal(SIGALRM, manage_alarm);

      while (attempts < 4) {
        // Send SET command
        send_control_frame(fd, TRANS_A, SET_C);
        printf("SET Command sent\n");

        // Set alarm for 3 seconds
        if(flag){
          alarm(3);
          flag=0;
        }

        // Setup receiving UA message
        if (receive_control_frame(fd, TRANS_A) ==  UA_C) {
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
      send_control_frame(fd, TRANS_A, UA_C);
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
    unsigned char command = receive_control_frame(fd, TRANS_A);

    switch(command) {
      case RR_C0:
      case REJ_C0:
        if(DATA_C == DATA_C1) {
          DATA_C = DATA_C0;
          printf("Receiver ready. Data transmitted.\n");
          return num_written_bytes;
        }
        else {
          printf("Reject. Attempting to retransmit data.\n");
        }
      break;
      case RR_C1:
      case REJ_C1:
        if(DATA_C == DATA_C0) {
          DATA_C = DATA_C1;
          printf("Receiver ready. Data transmitted.\n");
          return num_written_bytes;
        }
        else {
          printf("Reject. Attempting to retransmit data.\n");
        }
      break;
      default:
        printf("Command not received. Attempting to retransmit data.\n");
      break;
    }
  }

  return -1;
}

int send_data_frame(int fd, char * buffer, int length) {
  unsigned int index = 4;
  unsigned char frame[DATA_FRAME_LEN + length*2], bbc2 = 0;

  frame[0] = FLAG;
  frame[1] = TRANS_A;
  frame[2] = DATA_C;
  frame[3] = frame[1]^frame[2];

  // Generating BBC2
  for(int i = 0; i < length; i++) {
    bbc2 ^= buffer[i];
  }

  // Byte Stuffing - Data
  for(int i = 0; i < length; i++) {

    switch (buffer[i]) {
      case FLAG:
      case ESC:
        frame[index++] = ESC;
        frame[index++] = buffer[i]^BST_BYTE;
      break;
      default:
        frame[index++] = buffer[i];
      break;
    }
  }

  // Byte Stuffing - BBC2
  switch (bbc2) {
    case FLAG:
    case ESC:
      frame[index++] = ESC;
      frame[index++] = bbc2^BST_BYTE;
      frame[index] = FLAG;
    break;
    default:
      frame[index++] = bbc2;
      frame[index] = FLAG;
    break;
  }

  return write_serial(fd, frame, index);
}


////////////////////////////////////////////////////////////////////////////////
///////////////////////// Data Transmission - Receiver /////////////////////////
////////////////////////////////////////////////////////////////////////////////


int llread(int fd, char* buffer) {
  int length;
  unsigned char data_c, tmp_data_c = DATA_C, command;

  while(tmp_data_c == DATA_C) {
    length = receive_data_frame(fd, &data_c, buffer);

    if(length > -1) {
      command = data_c == DATA_C0 ? RR_C1 : RR_C0;

      if(DATA_C == data_c) {
          DATA_C = data_c == DATA_C0 ? DATA_C1 : DATA_C0;
      }
      else {
        printf("Duplicate frame\n");
      }
    }
    else {
      if(DATA_C == data_c) {
        command = data_c == DATA_C0 ? REJ_C0 : REJ_C1;
      }
      else {
        command = DATA_C == DATA_C0 ? RR_C0 : RR_C1;
        printf("Duplicate frame\n");
      }

    }

    send_control_frame(fd, TRANS_A, command);
  }

  return length;
}


int receive_data_frame(int fd, unsigned char * data_c, char * data) {
  unsigned int index = 0;
  unsigned char byte, bbc2 = 0;

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
        if(byte == FLAG){
          state = FLAG_REC;
        }
        else if(byte == DATA_C0 || byte == DATA_C1) {
          *data_c = byte;
          state = C_REC;
        }
        else
          state = START;
      break;
      case C_REC:
        if(byte == (TRANS_A^(*data_c))) {
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
          // Destuffing byte operation
          if(byte == ESC) {
            // Reads next byte
            read_serial(fd, &byte, 1);

            // Destuffs read byte
            byte ^= BST_BYTE;
          }

          bbc2 ^= byte;
          data[index++] = byte;
        }
      break;
      case END:
      break;
    }
  }

  if(bbc2 == 0) {
    return index;
  }

  return -1;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////// Termination of connection ////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int llclose(int fd) {
  // Reset global variables
  attempts = 1;
  flag = 1;

  switch (userType) {
    case TRANSMITTER:
      while (attempts < 4) {
        // Send SET command
        send_control_frame(fd, TRANS_A, DISC_C);
        printf("DISC Command sent\n");

        // Set alarm for 3 seconds
        if (flag){
          alarm(3);
          flag=0;
        }

        // Setup receiving DISC message
        if (receive_control_frame(fd, TRANS_A) ==  DISC_C) {
          printf("DISC Command received\n");
          send_control_frame(fd, TRANS_A, UA_C);
          printf("UA Command sent\n");
          break;
        }
        else {
          printf("DISC Command not received. Attempting to retransmit command.\n");
        }
      }

      if (attempts >= 4)
        printf("DISC Command not received\n");
    break;
    case RECEIVER:
      // Setup receiving DISC message
      while (receive_control_frame(fd, TRANS_A) != DISC_C);
      printf("DISC Command received\n");

      // Send DISC response
      send_control_frame(fd, TRANS_A, DISC_C);
      printf("DISC Command sent\n");

      // Setup receiving UA message
      while (receive_control_frame(fd, TRANS_A) != UA_C);
      printf("UA Command received\n");
    break;
    default:
      return -1;
    break;
  }

  close_serial(fd, 2);

  return 0;
}
