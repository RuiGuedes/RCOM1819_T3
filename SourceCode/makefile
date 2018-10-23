CC = gcc
CFLAGS = -Wall
DEPS = datalink.h serialconfig.h application.h
TRANSMITTER_OBJ = transmitter.o serialconfig.o datalink.o application.o
RECEIVER_OBJ = receiver.o serialconfig.o datalink.o application.o

all: transmitter receiver

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


transmitter: $(TRANSMITTER_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

receiver:  $(RECEIVER_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f transmitter receiver
	rm -f $(OBJ) *.o
