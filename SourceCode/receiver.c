#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "serialconfig.h"
#include "datalink.h"
#include "application.h"

int main(int argc, char** argv)
{
  if ( (argc < 2) ||
  ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  (strcmp("/dev/ttyS1", argv[1])!=0) &&
  (strcmp("/dev/ttyS2", argv[1])!=0) )) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  char filename[255];
  unsigned int file_length = 0;
  char buffer[file_length];

  receive_file(argv[1], filename, buffer, &file_length);

  // Open file to be sent
	FILE* file = fopen(filename, "w");
	if (!file) {
		printf("ERROR: File: %s could not be opened.\n", argv[2]);
		return -1;
	}

  for(int i = 0; i < file_length; i++) {
    fwrite(buffer+i, 1, 1, file);
  }

  return 0;
}
