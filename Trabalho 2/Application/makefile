CC = gcc 
CFLAGS = -std=c99 -Wall -D_POSIX_C_SOURCE 
DEPS = url_parser.h client_tcp.h
DOWNLOAD_OBJ = download.o url_parser.o client_tcp.o

all: download

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

download: $(DOWNLOAD_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f download
	rm -f $(OBJ) *.o
