/*Non-Canonical Input Processing*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "serialconfig.h"

#define FLAG    0x7E

#define EMIT_A  0x03
#define REC_A  0x01

#define SET_C   0x03
#define UA_C   0x07

int main(int argc, char** argv)
{
    if ( (argc < 2) ||
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }

    int fd = init_serial_n_canon(argv[1]);

    // Setup receiving SET message
    unsigned char byte;

    enum set_states {START, FLAG_REC, A_REC, C_REC, BCC_OK, END};
    enum set_states state = START;

    while (state != END) {
      read(fd, &byte, 1);

      switch(state) {
        case START:
          if (byte == FLAG)
            state = FLAG_REC;
        break;
        case FLAG_REC:
          if (byte == EMIT_A)
            state = A_REC;
          else if (byte != FLAG)
            state = START;
        break;
        case A_REC:
          if (byte == SET_C)
            state = C_REC;
          else if (byte == FLAG)
            state = FLAG_REC;
          else
            state = START;
        break;
        case C_REC:
          if ((EMIT_A^SET_C) == byte)
            state = BCC_OK;
          else if (byte == FLAG)
            state = FLAG_REC;
          else
            state = START;
        break;
        case BCC_OK:
          if (byte == FLAG)
            state = END;
          else
            state = START;
        break;
        case END:
          break;
      }
    }

    printf("SET Command received\n");

    // SEND UA COMMAND
    unsigned char UA[5];

    UA[0] = FLAG;
    UA[1] = REC_A;
    UA[2] = UA_C;
    UA[3] = UA[1]^UA[2];
    UA[4] = FLAG;

    int writtenBytes = 0;

    while(writtenBytes < 5) {
      writtenBytes += write(fd,UA + writtenBytes, 6 - writtenBytes);
    }

    close_serial(fd, 2);

    return 0;
}
