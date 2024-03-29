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

  // Display's initial information
  printf("\n###############\n");
  printf("## RECEIVER ##\n");
  printf("###############\n\n");
  printf("--- Settings ---\n\n");
  printf("Serial port: %s\n", argv[1]);
  printf("Max attempts: %d\n", 3);
  printf("Timeout: %d seconds\n\n", 3);

  // Initialization to avoid same random numbers
  srand(time(NULL));

	time_t begin = time(NULL);

	receive_file(argv[1]);

	printf("Execution time: %f\n", difftime(time(NULL), begin));

  return 0;
}
