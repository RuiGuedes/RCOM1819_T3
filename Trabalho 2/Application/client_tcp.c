#include "client_tcp.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// API - Functions /////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

int llopen(URL * url_info, int server_port) {
    //Variables
    int	socket_fd;
	struct sockaddr_in server_addr;
	
	// Server address handling 
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)url_info->IP->h_addr_list[0])));	/* 32 bit Internet address network byte ordered */
	server_addr.sin_port = htons(server_port);		                                                        /* Server TCP port must be network byte ordered */
    
	// Open a TCP socket 
	if((socket_fd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    	perror("socket()");
        exit(1);
    }

	// Connect to the server 
    if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect()");
		exit(1);
	}

    // Read response
    if(server_port == 21) {
        char response[SERVER_RESPONSE_LEN];
        llread(socket_fd, response, sizeof(response));
    }

    return socket_fd;
}

int llwrite(int socket_fd, char * cmd, size_t cmd_size) {
    // Number of bytes write to the socket file descriptor
    int bytes = 0;

    if((bytes = write(socket_fd, cmd, cmd_size)) <= 0) {
        printf("Error :: llwrite()");
        exit(1);
    }
    
    return bytes;
}

int llread(int socket_fd, char * response, size_t response_size) {
    // Open socket for reading
    FILE* fp = fdopen(socket_fd, "r");
    
    // Read socket messages relative to previous commands
    do {
		memset(response, 0, response_size);
		response = fgets(response, response_size, fp);
		printf("%s", response);
	} while (!('1' <= response[0] && response[0] <= '5') || response[3] != ' ');

	return 0;
}

int llclose(int socket_fd) {
    // Variables
    char cmd[CMD_LEN];
    sprintf(cmd, "quit\r\n");

    // Send "quit" command
    llwrite(socket_fd, cmd, strlen(cmd));

    // Read response
    char response[SERVER_RESPONSE_LEN];
    llread(socket_fd, response, sizeof(response));

    // Closes socket connection
    if(close(socket_fd)) {
        perror("close()");
        exit(1);
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// High-level functions ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

int login(URL * url_info, int socket_fd) {
    // Variables
    char credentials[CREDENTIALS_LEN];

	// Send username 
	sprintf(credentials, "user %s\r\n", url_info->user);
    
    if(llwrite(socket_fd, credentials, strlen(credentials)) == -1) {
		printf("Error :: Sending username info.\n");
		return 1;
	}

    // Read user response
    char user_response[SERVER_RESPONSE_LEN];
    llread(socket_fd, user_response, sizeof(user_response));

	// Reset buffer and send password
 	memset(credentials, 0, sizeof(credentials));
	sprintf(credentials, "pass %s\r\n", url_info->password);

    if(llwrite(socket_fd, credentials, strlen(credentials)) == -1) {
		printf("Error :: Sending password info.\n");
		return 1;
	}

    // Read password response
    char password_response[SERVER_RESPONSE_LEN];
    llread(socket_fd, password_response, sizeof(password_response));

    return 0;
}

int set_cwd(URL * url_info, int socket_fd) {
    // Variables
    char cmd[SERVER_RESPONSE_LEN];

    // In case that no directory was passed by on URL 
    if(strcmp(url_info->url_path, "") == 0){
        return 0;
    }

	// Send cwd command 
	sprintf(cmd, "cwd %s\r\n", url_info->url_path);
    
    if(llwrite(socket_fd, cmd, strlen(cmd)) == -1) {
		printf("Error :: Sending cwd command.\n");
		return 1;
	}
    
    // Read response
    char response[SERVER_RESPONSE_LEN];
    llread(socket_fd, response, sizeof(response));

    return 0;
}

int set_passive_mode(int socket_fd, int * port) {
    // Variables
    const char left_slash = '('; const char right_slash = ')';
    char cmd[SERVER_RESPONSE_LEN];

	// Send passive mode command 
	sprintf(cmd, "pasv\r\n");
    
    if(llwrite(socket_fd, cmd, strlen(cmd)) == -1) {
		printf("Error :: Sending pasv command.\n");
		return 1;
	}
    
    // Read response
    char response[SERVER_RESPONSE_LEN];
    llread(socket_fd, response, sizeof(response));

    // Store information
    char server_info[256];
    char * first_bracket; first_bracket = strchr(response, left_slash);
    char * last_bracket; last_bracket = strrchr(response, right_slash);

    memcpy(server_info, first_bracket + 1, strlen(first_bracket) - strlen(last_bracket) - 1);
    server_info[strlen(first_bracket) - strlen(last_bracket)] = '\0';

    // Tokenize server info variables
    int index = 0;
    int info[6];
    char * token;
    const char s[2] = ",";
    
    // Get the first token
    token = strtok(server_info, s);
    
    // Walk through other tokens
    while( token != NULL ) {
        info[index++] = atoi(token);   
        token = strtok(NULL, s);
    }
    
    // Set new port value 
    *port = info[4]*256 + info[5];

    return 0;
}

int send_retr_cmd(URL * url_info, int socket_fd) {
    // Variables
    char cmd[SERVER_RESPONSE_LEN];

	// Send retrieve command
	sprintf(cmd, "retr %s\r\n", url_info->filename);
    
    if(llwrite(socket_fd, cmd, strlen(cmd)) == -1) {
		printf("Error :: Sending retr command.\n");
		return 1;
	}
    
    // Read response
    char response[SERVER_RESPONSE_LEN];
    llread(socket_fd, response, sizeof(response));

    return 0;
}

int download(URL * url_info, int data_socket_fd) {
    // Variables
    int bytes;
    char buffer[1024];
    FILE * downloaded_file;

	if (!(downloaded_file = fopen(url_info->filename, "w"))) {
		printf("ERROR: Cannot create/open new file.\n");
		return 1;
	}

	while ((bytes = read(data_socket_fd, buffer, sizeof(buffer)))) {
		if (bytes < 0) {
			printf("ERROR: Nothing was received from data socket fd.\n");
			return 1;
		}

		if ((bytes = fwrite(buffer, bytes, 1, downloaded_file)) < 0) {
			printf("ERROR: Cannot write data in file.\n");
			return 1;
		}
	}

	fclose(downloaded_file);
	close(data_socket_fd);
    
    return 0;
}