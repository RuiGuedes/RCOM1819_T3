#ifndef URL_PARSER_H
#define URL_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h> 
#include <sys/types.h>

// Defines

#define TCP_PORT 21
#define HEADER_LEN 6

// RFC 1738 [Section 3] - FTP Conventions

#define PROTOCOL_HEADER "ftp://"
#define DEFAUlT_USER "anonymous"
#define DEFAUlT_PASSWORD "mail@domain"

// URL structure information

typedef struct {
    char user[256];
    char password[256];
    char host[256];
    struct hostent * IP;
    char url_path[256];
    char filename[256];
} URL;

/**
 *  Parses the given URL and stores its information on url_info struct
 * 
 *  Argument: url       - URL to be parsed
 *  Argument: url_info  - Struct where URL information will be stored
 */
int parse_url(char * url, URL * url_info);

/**
 *  Retrieves host information and stores its information on url_info struct
 *  
 *  Argument: host      - Specifies which host it's needed information
 *  Argument: url_info  - Struct where URL information will be stored
 */
int get_host_IP(char * host, URL * url_info);

#endif