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

#define DEFAULT   "\033[0m"
#define BLACK     "\033[30m"      /* Black */
#define RED       "\033[31m"      /* Red */
#define GREEN     "\033[32m"      /* Green */
#define YELLOW    "\033[33m"      /* Yellow */
#define BLUE      "\033[34m"      /* Blue */
#define MAGENTA   "\033[35m"      /* Magenta */
#define CYAN      "\033[36m"      /* Cyan */
#define WHITE     "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m" /* Bold White */

/*
    Main function of transmitter.

    Brief: It opens, reads and stores the file content, which name is passed by parameter, and sends it to the serial port also passed by parameter.
*/
int main(int argc, char** argv)
{
  if ( (argc < 3) ||
  ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  (strcmp("/dev/ttyS1", argv[1])!=0) &&
  (strcmp("/dev/ttyS2", argv[1])!=0) )) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  // Open file to be sent
  struct stat buffer;
  int length = 0;

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

  // Display's initial information
  printf("\n#################\n");
  printf("## TRANSMITTER ##\n");
  printf("#################\n\n");

  printf("--- Settings ---\n\n");
  printf("Serial port: %s\n", argv[1]);
  printf("Max attempts: %d\n", 3);
  printf("Timeout: %d seconds\n", 3);
  printf("File to be transmitted: %s\n", argv[2]);
  printf("File size: %d bytes\n\n", length);


  send_file(argv[1], argv[2], file_content, length);

  return 0;
}
