// ALL SERIAL PORT RELATED STUFF
#ifndef _SERIAL_CONFIG
#define _SERIAL_CONFIG

#include <termios.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

struct termios oldtio; // Original configuration

/*
  Sets up the serial port in non-canonical mode (Based off of source code provided)

  Return: file descriptor  of serial port
*/
int init_serial_n_canon(char* serialpath);

/*
  Closes the serial port, setting it back to original configuration

  Argument 'fd': file descriptor of serial port to close
  Argument 'wait_time': time in seconds to wait before resetting the config (Give time for all the inserted bits to be transmitted)
*/
void close_serial(int fd, int wait_time);

#endif
