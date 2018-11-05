#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "serialconfig.h"

int init_serial_n_canon(char* serialpath) {
  int fd;
  struct termios newtio;
  
  /*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
  */
  fd = open(serialpath, O_RDWR | O_NOCTTY );
  if (fd <0) { perror(serialpath); exit(-1); }

  if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
    perror("tcgetattr(new)");
    exit(-1);
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME]    = 5;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */

  /*
  VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
  leitura do(s) proximo(s) caracter(es)
  */

  tcflush(fd, TCIOFLUSH);

  if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");

  return fd;
}

int write_serial(int fd, unsigned char* buffer, int length) {
  int writtenBytes = 0;

  while(writtenBytes < length)
    writtenBytes += write(fd, buffer + writtenBytes, length + 1 - writtenBytes);

  return writtenBytes;
}

void read_serial(int fd, unsigned char* buffer, int length) {
  read(fd, buffer, length);
}

void close_serial(int fd, int wait_time) {
  sleep(wait_time);

  if (tcsetattr(fd,TCSANOW,&oldtio) == -1) {
    perror("tcsetattr(old)");
    exit(-1);
  };
  close(fd);
}
