#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "serialconfig.h"
#include "datalink.h"
#include "application.h"

int main(int argc, char** argv)
{
  if ( (argc < 3) ||
  ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  (strcmp("/dev/ttyS1", argv[1])!=0) &&
  (strcmp("/dev/ttyS2", argv[1])!=0) )) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  struct stat buffer;
  int length = 0;

  // Open file to be sent
	FILE* file = fopen(argv[2], "r");
	if (!file) {
		printf("ERROR: File: %s could not be opened.\n", argv[2]);
		return -1;
	}

  // Get file size
  stat(argv[2], &buffer);
  int size = buffer.st_size;

  char file_content[size];

  while (!feof(file)) {
      length += fread(file_content, 1, sizeof(file_content), file);
  }

  send_file(argv[1], argv[2], file_content, length);

  return 0;
}
