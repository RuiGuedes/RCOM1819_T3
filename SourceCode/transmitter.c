/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7e
#define EMIT_A 0x03
#define REC_A 0x01
#define SET_C 0x03
#define UA_C 0x07

volatile int STOP=FALSE;

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];
    int i = 0, sum = 0, speed = 0;

    if ( (argc < 2) ||
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */


    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */



  /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) pr�ximo(s) caracter(es)
  */

    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");

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
          if ((A^C) == byte)
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

    sleep(2);

    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);
    return 0;
}
