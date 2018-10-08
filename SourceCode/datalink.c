#include "datalink.h"
#include "serialconfig.h"

void send_control_frame(int fd, int addr_byte, int ctrl_byte) {
  unsigned char frame[CTRL_FRAME_LEN];

  frame[0] = FLAG;
  frame[1] = addr_byte;
  frame[2] = ctrl_byte;
  frame[3] = frame[1]^frame[2];
  frame[4] = FLAG;

  write_serial(fd, frame, CTRL_FRAME_LEN);
}

void receive_control_frame(int fd, int addr_byte, int ctrl_byte) {
  unsigned char byte;
  int bbc_byte = addr_byte ^ ctrl_byte;

  enum set_states {START, FLAG_REC, A_REC, C_REC, BCC_OK, END};
  enum set_states state = START;

  while (state != END) {
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
}
