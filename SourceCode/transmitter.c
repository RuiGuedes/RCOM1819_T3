/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "serialconfig.h"

#define FLAG 0x7e
#define EMIT_A 0x03
#define REC_A 0x01
#define SET_C 0x03
#define UA_C 0x07

int main(int argc, char** argv)
{
    int i = 0, sum = 0, speed = 0;

    if ( (argc < 2) ||
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }

    int fd = init_serial_n_canon(argv[1]);

    unsigned char SET[5];

    SET[0] = FLAG;
    SET[1] = EMIT_A;
    SET[2] = SET_C;
    SET[3] = SET[1]^SET[2];
    SET[4] = FLAG;

    int writtenBytes = 0;

    while(writtenBytes < 5) {
      writtenBytes += write(fd,SET + writtenBytes, 6 - writtenBytes);
    }

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
          if (byte == REC_A)
            state = A_REC;
          else if (byte != FLAG)
            state = START;
        break;
        case A_REC:
          if (byte == UA_C)
            state = C_REC;
          else if (byte == FLAG)
            state = FLAG_REC;
          else
            state = START;
        break;
        case C_REC:
          if ((REC_A^UA_C) == byte)
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

    printf("UA Command received\n");

    /*
    gets(buf);

    int length = strlen(buf);
    int writtenBytes = 0;

    while(writtenBytes < length) {
      writtenBytes += write(fd,buf + writtenBytes,length+1);
    }

    do {
      read(fd, buf + i, 1);
    } while (buf[i++] != '\0');

    printf("%s\n", buf);
    */

    close_serial(fd, 2);

    return 0;
}
