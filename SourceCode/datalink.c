#include "datalink.h"
#include "serialconfig.h"

// Init extern global variables

int flag = 1;
int attempts = 1;
int DATA_C = DATA_C1;

void llwrite(int fd, char * buffer, int length) {

  send_data_frame(fd, buffer, length);

  //RR OU REJ
  // TIMEOUT

}

void send_data_frame(int fd, char * buffer, int length) {
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

  write_serial(fd, frame, DATA_FRAME_LEN + length);
}

int receive_data_frame(int fd, int addr_byte, int ctrl_byte) {
  unsigned char byte, bbc2 = 0;
  unsigned char bbc_byte = addr_byte ^ ctrl_byte;

  enum set_states {START, FLAG_REC, A_REC, C_REC, DATA_REC, BCC_OK, END};
  enum set_states state = START;

  while (state != END) {
    if(flag)
      return INSUCCESS;

    read_serial(fd, &byte, 1);

    switch(state) {
      case START:
        if      (byte == FLAG)        state = FLAG_REC;
        break;
      case FLAG_REC:
        if      (byte == addr_byte)   state = A_REC;
        else if (byte != FLAG)        state = START;
        break;
      case A_REC:
        if      (byte == ctrl_byte)   state = C_REC;
        else if (byte == FLAG)        state = FLAG_REC;
        else                          state = START;
        break;
      case C_REC:
        if      (byte == bbc_byte)    state = BCC_OK;
        else if (byte == FLAG)        state = FLAG_REC;
        else                          state = START;
        break;
      case BCC_OK:
        if      (byte == FLAG)        state = DATA_REC;
        else                          state = START;
        break;
      case DATA_REC:
        if      (byte == FLAG)        state = END;
        else {
            bbc2 ^= byte;
            //Store on buffer
        }
        break;
      case END:
        break;
    }
  }
  return SUCCESS;
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

int receive_control_frame(int fd, int addr_byte, int ctrl_byte) {
  unsigned char byte;
  int bbc_byte = addr_byte ^ ctrl_byte;

  enum set_states {START, FLAG_REC, A_REC, C_REC, BCC_OK, END};
  enum set_states state = START;

  while (state != END) {
    if(flag)
      return INSUCCESS;

    read_serial(fd, &byte, 1);

    switch(state) {
      case START:
        if      (byte == FLAG)        state = FLAG_REC;
        break;
      case FLAG_REC:
        if      (byte == addr_byte)   state = A_REC;
        else if (byte != FLAG)        state = START;
        break;
      case A_REC:
        if      (byte == ctrl_byte)   state = C_REC;
        else if (byte == FLAG)        state = FLAG_REC;
        else                          state = START;
        break;
      case C_REC:
        if      (byte == bbc_byte)    state = BCC_OK;
        else if (byte == FLAG)        state = FLAG_REC;
        else                          state = START;
        break;
      case BCC_OK:
        if      (byte == FLAG)        state = END;
        else                          state = START;
        break;
      case END:
        break;
    }
  }
  return SUCCESS;
}
