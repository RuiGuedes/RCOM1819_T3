#include "url_parser.h"
#include "client_tcp.h"

int main(int argc, char ** argv) {

    // Function call validation
    if(argc != 2) {
        printf("Usage :: download ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(1);
    }

    /////////////////
    // Parsing URL //
    /////////////////
    URL url_info;

    if(parse_url(argv[1], &url_info) != 0) {
        printf("Error while parsing URL :: Invalid URL\n");
        exit(1);
    }    

    get_host_IP(url_info.host, &url_info);

    ///////////////////////////
    // Connection & Download //
    ///////////////////////////
    
    // Initiliazes connection
    int socket_fd = llopen(&url_info, TCP_PORT);
    
    // Log's in user
    login(&url_info, socket_fd);

    // Updates server current working directory
    set_cwd(&url_info, socket_fd);

    // Set server mode to - passive - and retrieves new port where file will be downloaded
    int new_port;
    set_passive_mode(socket_fd, &new_port);

    // Initiliazes connection on the new port
    int data_socket_fd = llopen(&url_info, new_port);

    // Send's retrieve command
    send_retr_cmd(&url_info, socket_fd);

    // Download and store file
    download(&url_info, data_socket_fd);

    // Terminate connection
    llclose(socket_fd); 

    return 0;
}