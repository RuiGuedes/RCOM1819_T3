all: transmitter receiver

transmitter: transmitter.c datalink.h serialconfig.h
	gcc -o transmitter transmitter.c datalink.c serialconfig.c -Wall

receiver: receiver.c datalink.h serialconfig.h
	gcc -o receiver receiver.c datalink.c serialconfig.c -Wall
