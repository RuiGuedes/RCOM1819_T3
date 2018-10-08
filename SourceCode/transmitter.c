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
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }
    int fd = init_serial_n_canon(argv[1]);

    // Send SET command
    send_control_frame(fd, TRANS_A, SET_C);

    // Setup receiving UA message
    receive_control_frame(fd, TRANS_A, UA_C);
    printf("UA Command received\n");

    close_serial(fd, 2);
    return 0;
}
