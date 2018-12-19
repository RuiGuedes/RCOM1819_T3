#include "url_parser.h"

int parse_url(char * url, URL * url_info) {
    // Variables declaration
    int  user_presence = 1;
    char header[HEADER_LEN + 2];
    const char slash = '/';

    // Retrives URL header
    memcpy(header, url, HEADER_LEN);
    header[HEADER_LEN + 1] = '\0';
    
    if(strcmp(header, PROTOCOL_HEADER) != 0) {
        printf("Invalid URL :: URL header must be [ftp://]\n");
        return 1;
    }
    
    // Remove header from URL
    char new_url[256];
    memcpy(new_url, url + HEADER_LEN, strlen(url) - HEADER_LEN + 1);
    new_url[strlen(url) - HEADER_LEN] = '\0';

    // Check host, user and password existence
    char * password = strchr(new_url, ':');
    char * host = strchr(new_url, '@');

    // User is not present on the URL
    if(!host) {
        user_presence = 0;
    }
    
    // Store information
    char * first_slash; first_slash = strchr(new_url, slash);
    char * last_slash; last_slash = strrchr(new_url, slash);

    if(user_presence) {
        if(password) {
            memcpy(url_info->user, new_url, strlen(new_url) - strlen(password)); // Stores user
            url_info->user[strlen(new_url) - strlen(password)] = '\0';

            memcpy(url_info->password, password + 1, strlen(password) - strlen(host) - 1); // Stores user password
            url_info->password[strlen(password) - strlen(host) -1] = '\0';
        }
        else {
            memcpy(url_info->user, new_url, strlen(new_url) - strlen(host));
            url_info->user[strlen(new_url) - strlen(host)] = '\0';

            // Retrieve user password
            char pass[MAX_PASSWD_LEN + 1];
            get_user_password(pass);

            memcpy(url_info->password, pass, strlen(pass));
            url_info->password[strlen(pass)] = '\0';
        }
    }
    else {
        memcpy(url_info->user, DEFAUlT_USER, strlen(DEFAUlT_USER)); url_info->user[strlen(DEFAUlT_USER)] = '\0';
        memcpy(url_info->password, DEFAUlT_PASSWORD, strlen(DEFAUlT_PASSWORD)); url_info->password[strlen(DEFAUlT_PASSWORD)] = '\0'; 
    }

    // Display user information
    printf("[Username] -> %s\n", url_info->user);
    if((password && user_presence) || (!user_presence)) {
        printf("[Password] -> %s\n\n", url_info->password);
    }
    else {
        printf("\n");
    }

    if(host) {
        memcpy(url_info->host, host + 1, strlen(host) - strlen(first_slash) - 1);
        url_info->host[strlen(host) - strlen(first_slash)] = '\0';
    }
    else {
        memcpy(url_info->host, new_url, strlen(new_url) - strlen(first_slash));
        url_info->host[strlen(new_url) - strlen(first_slash) + 1] = '\0';
    }
    
    memcpy(url_info->url_path, first_slash + 1, strlen(first_slash) - strlen(last_slash));
    url_info->url_path[strlen(first_slash) - strlen(last_slash)] = '\0';
    
    memcpy(url_info->filename, last_slash + 1, strlen(last_slash) );   
    url_info->filename[strlen(last_slash) + 1] = '\0';

    return(0);
}

int get_host_IP(char * host, URL * url_info) {
    if ((url_info->IP = gethostbyname(host)) == NULL) {  
        printf("Error :: gethostbyname() :: Check your connection\n");
        exit(1);
    }

    printf("Host name  : %s\n", url_info->IP->h_name);
    printf("IP Address : %s\n\n", inet_ntoa(*((struct in_addr *)url_info->IP->h_addr_list[0])));

    return 0;
}

int get_user_password(char * password) {
    // Variables
    int index = 0;
    char ch, echo = '*';
    struct termios new_term, oldterm;
    
    write(STDOUT_FILENO, "Password ? ", 11);
    
    tcgetattr(STDIN_FILENO, &oldterm);
    new_term = oldterm;
    new_term.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term);
    
    while (index < MAX_PASSWD_LEN && read(STDIN_FILENO, &ch, 1) && ch != '\n') {
        password[index++] = ch;
        write(STDOUT_FILENO, &echo, 1);
    }
    
    password[index] = 0;
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldterm);
    printf("\n\n");

    return 0;
}
