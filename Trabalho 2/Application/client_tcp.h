#ifndef CLIENT_TCP_H
#define CLIENT_TCP_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <strings.h>
#include <sys/ioctl.h>
#include "url_parser.h"

/////////////
// Defines //
/////////////

#define NO_FLAGS            0
#define CMD_LEN             512
#define SERVER_RESPONSE_LEN 1024
#define CREDENTIALS_LEN     512

/////////////////////
// API - Functions //
/////////////////////

/**
 *  Initializes TCP socket and connects it to the server
 * 
 *  Argument: url_info    - Struct where URL information is stored
 *  Argument: server_port - Server port where connection will be made
 */
int llopen(URL * url_info, int server_port);

/**
 *  Terminates TCP socket connection to the server
 * 
 *  Argument: socket_fd - TCP socket file descriptor
 */
int llclose(int socket_fd);

/**
 *  Writes command to the server through TCP socket
 * 
 *  Argument: socket_fd - TCP socket file descriptor
 *  Argument: cmd       - Command to be sent
 *  Argument: cmd_size  - Command size
 */
int llwrite(int socket_fd, char * cmd, size_t cmd_size);

/**
 *  Retrieves server response to a certain command via TCP socket
 * 
 *  Argument: socket_fd     - TCP socket file descriptor
 *  Argument: response      - Response to a certain command
 *  Argument: response_size - Response size
 */
int llread(int socket_fd, char * response, size_t response_size);

//////////////////////////
// High-level functions //
//////////////////////////

/**
 *  Login in the server using URL parsed info (user and password)
 * 
 *  Argument: url_info    - Struct where URL information is stored
 *  Argument: socket_fd   - TCP socket file descriptor
 */
int login(URL * url_info, int socket_fd);

/**
 *  Set's the server current working directory to the one provided in the URL
 * 
 *  Argument: url_info    - Struct where URL information is stored
 *  Argument: socket_fd   - TCP socket file descriptor
 */
int set_cwd(URL * url_info, int socket_fd);

/**
 *  Set's server to passive mode
 * 
 *  Argument: socket_fd   - TCP socket file descriptor
 *  Argument: port        - Port from where data will be read
 */
int set_passive_mode(int socket_fd, int * port);

/**
 *  Retrives a certain file which name is present on url_info struct
 * 
 *  Argument: url_info    - Struct where URL information is stored
 *  Argument: socket_fd   - TCP socket file descriptor
 */
int send_retr_cmd(URL * url_info, int socket_fd);

/**
 *  Downloads the file and stores it in current working directory where the program is being launched
 * 
 *  Argument: url_info    - Struct where URL information is stored
 *  Argument: socket_fd   - TCP socket file descriptor
 */
int download(URL * url_info, int data_socket_fd);

#endif