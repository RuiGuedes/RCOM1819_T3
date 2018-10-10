#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "serialconfig.h"
#include "datalink.h"

void manage_alarm() {
  flag=1;
  attempts++;
}

int main(int argc, char** argv)
{
  if ( (argc < 2) ||
  ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  (strcmp("/dev/ttyS1", argv[1])!=0) )) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  int fd = init_serial_n_canon(argv[1]);

  (void) signal(SIGALRM, manage_alarm);

  while(attempts < 4) {
    // Send SET command
    send_control_frame(fd, TRANS_A, SET_C);

    // Set alarm for 3 seconds
    if(flag){
      alarm(3);
      flag=0;
    }

    // Setup receiving UA message
    if(receive_control_frame(fd, TRANS_A, UA_C) == SUCCESS) {
      printf("UA Command received\n");
      break;
    }

  }

  if(attempts >= 4)
    printf("UA Command not received\n");

  close_serial(fd, 2);
  return 0;
}
