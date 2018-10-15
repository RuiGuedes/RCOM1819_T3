#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "serialconfig.h"
#include "datalink.h"

int main(int argc, char** argv)
{
  if ( (argc < 2) ||
  ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  (strcmp("/dev/ttyS1", argv[1])!=0) &&
  (strcmp("/dev/ttyS2", argv[1])!=0) )) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  int port = init_serial_n_canon(argv[1]);

  llopen(port, RECEIVER);

  return 0;
}
