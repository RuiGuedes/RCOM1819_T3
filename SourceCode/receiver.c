#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "serialconfig.h"
#include "datalink.h"
#include "application.h"

/*
    Main function of receiver

    Brief: Validates function call and call the respective function to receive a certain file via serial port.
*/
int main(int argc, char** argv)
{
  if ( (argc < 2) ||
  ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  (strcmp("/dev/ttyS1", argv[1])!=0) &&
  (strcmp("/dev/ttyS2", argv[1])!=0) )) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  clock_t begin = clock();

  receive_file(argv[1]);

  clock_t end = clock();

  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

  printf("Execution time: %lf\n", time_spent);

  return 0;
}
