CC = gcc 
CFLAGS = -std=c99 -Wall -D_POSIX_C_SOURCE 
DEPS = URL_parser.h
DOWNLOAD_OBJ = download.o URL_parser.o

all: download

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

download: $(DOWNLOAD_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f download
	rm -f $(OBJ) *.o
