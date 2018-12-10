#include "URL_parser.h"

int main(int argc, char ** argv) {

    if(argc != 2) {
        printf("Usage :: dowload ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(1);
    }

    // ---- Parsing URL - INIT ----
    URL url_info;

    if(parse_url(argv[1], &url_info) != 0) {
        printf("Error while parsing URL :: Invalid URL\n");
        exit(1);
    }    

    get_host_IP(url_info.host, &url_info);

    // ---- Parsing URL - END ----

    // ---- Connection & Download - INIT ----

    // ---- Connection & Download - END ----

    return 0;
}