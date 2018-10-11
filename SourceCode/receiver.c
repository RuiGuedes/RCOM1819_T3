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
    int fd = init_serial_n_canon(argv[1]);

    //Reset alarm FLAG
    flag = 0;

    // Setup receiving SET message
    receive_control_frame(fd, TRANS_A, SET_C);
    printf("SET Command received\n");

    // Send UA response
    send_control_frame(fd, TRANS_A, UA_C);

    close_serial(fd, 2);
    return 0;
}
