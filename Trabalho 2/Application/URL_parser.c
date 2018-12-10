#include "URL_parser.h"

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
    memcpy(new_url, url + HEADER_LEN, strlen(url) - HEADER_LEN);
    new_url[strlen(url) - HEADER_LEN - 1] = '\0';
    
    // Check host, user and password existence
    char * password = strchr(new_url, ':');
    char * host = strchr(new_url, '@');

    if(host) {
        if((host - new_url) == 0) {
            user_presence = 0;
        }      
    }
    else {
        printf("Invalid URL :: Missing [host] parameter\n");
        return 1;
    }
    
    // Store information
    char * first_slash; first_slash = strchr(new_url, slash);
    char * last_slash; last_slash = strrchr(new_url, slash);

    if(user_presence) {
        if(password) {
            memcpy(url_info->user, new_url, strlen(new_url) - strlen(password));
            memcpy(url_info->password, password + 1, strlen(password) - strlen(host) - 1); url_info->password[strlen(password) - strlen(host) -1] = '\0';
        }
        else {
            memcpy(url_info->user, new_url, strlen(new_url) - strlen(host));
            memcpy(url_info->password, DEFAUlT_PASSWORD, strlen(DEFAUlT_PASSWORD)); url_info->password[strlen(DEFAUlT_PASSWORD)] = '\0'; 
        }
    }
    else {
        memcpy(url_info->user, DEFAUlT_USER, strlen(DEFAUlT_USER)); url_info->user[strlen(DEFAUlT_USER)] = '\0';
        memcpy(url_info->password, DEFAUlT_PASSWORD, strlen(DEFAUlT_PASSWORD)); url_info->password[strlen(DEFAUlT_PASSWORD)] = '\0'; 
    }


    memcpy(url_info->host, host + 1, strlen(host) - strlen(first_slash) - 1);
    memcpy(url_info->url_path, first_slash, strlen(first_slash) - strlen(last_slash));
    memcpy(url_info->filename, last_slash + 1, strlen(last_slash));   

    return(0);
}


int get_host_IP(char * host, URL * url_info) {
    if ((url_info->IP = gethostbyname(host)) == NULL) {  
        printf("Error on gethostbyname function :: Invalid host\n");
        exit(1);
    }

    printf("Host name  : %s\n", url_info->IP->h_name);
    printf("IP Address : %s\n",inet_ntoa(*((struct in_addr *)url_info->IP->h_addr_list[0])));

    return 0;
}
