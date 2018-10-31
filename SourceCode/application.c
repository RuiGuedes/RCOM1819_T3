#include <stdio.h>
#include <string.h>
#include "datalink.h"
#include "application.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////// Control Packet Functions /////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int send_control_packet(int fd, int type, char * filename, unsigned int length) {
  unsigned int index = 0;
  char packet[CONTROL_PACKET_LEN + sizeof(filename) + sizeof(length)];

  if(strlen(filename) > 254) {
      printf("Filename: %s too large to fit in packet.\n", filename);
      return -1;
  }

  packet[index++] = type;

  packet[index++] = FILESIZE_T;
  packet[index++] = sizeof(length);
  packet[index++] = length & 0xFF;
  packet[index++] = (length >> 8) & 0xFF;
  packet[index++] = (length >> 16) & 0xFF;
  packet[index++] = (length >> 24) & 0xFF;

  packet[index++] = FILENAME_T;
  packet[index++] = strlen(filename) + 1;
  for(int i = 0; i <= strlen(filename); i++) {
      packet[index++] = filename[i];
  }

  return llwrite(fd, packet, index);
}

int receive_control_packet(int fd, int type, char * filename, unsigned int * file_length) {
    int tmp_length = 0;
    unsigned int index = 0;
    *file_length = 0xFFFFFFFF;
    char buffer[CONTROL_PACKET_LEN + 255 + sizeof(int)];

    int packet_length = llread(fd, buffer);

    if(type != buffer[index++]) {
      return -1;
    }

    while(index < packet_length) {

        switch (buffer[index++]) {
          case FILESIZE_T:
            tmp_length = buffer[index++];
            *file_length = ((buffer[index+3] << 24) | 0x00FFFFFF) & *file_length;
            *file_length = ((buffer[index+2] << 16) | 0x0000FFFF) & *file_length;
            *file_length = ((buffer[index+1] << 8) | 0x000000FF) & *file_length;
            *file_length = buffer[index] & *file_length;
            index += tmp_length;
          break;
          case FILENAME_T:
            tmp_length = buffer[index++];
            for(int i = 0; i < tmp_length; i++){
              filename[i] = buffer[index++];
            }
          break;
          default:
            return -1;
          break;
        }
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
///////////////////////// Data Packet Functions /////////////////////////////
/////////////////////////////////////////////////////////////////////////////

int send_data_packet(int fd, unsigned int N, char * buffer, unsigned int length) {
  if (length > MAX_DATA_LEN) {
    printf("Length overflow\n");
    return -1;
  }
  else if (length == 0) {
    return 0; // For safety reasons, best to not do anything in this case
  }

  unsigned int index = 0;
  char packet[DATA_PACKET_LEN + length];

  packet[index++] = DATA_C;
  packet[index++] = (unsigned char) N;
  packet[index++] = (char)(length / 256);
  packet[index++] = (char)(length % 256);

  for(int i = 0; i < length; i++) {
    packet[index++] = buffer[i];
  }

  return llwrite(fd, packet, index);
}

unsigned int receive_data_packet(int fd, char * buffer, int * buf_len) {
   unsigned int index = 0;
   char packet[DATA_PACKET_LEN + MAX_DATA_LEN];

   llread(fd, packet);

   if(packet[index++] != DATA_C) {
     printf("Wrong packet received. Should receive a data packet.\n");
     return -1;
   }

   unsigned int N = (unsigned char) packet[index++];

   *buf_len = 256 * (unsigned char)(packet[index]) + (unsigned char)(packet[index+1]);
   index += 2;

   for(int i = 0; i < *buf_len; i++) {
     buffer[i] = packet[index++];
   }

   return N;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////// API application functions /////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int send_file(char * port, char * filename, char * file_content, int length){

  // Establish connection
  int fd = llopen(port, TRANSMITTER);

  // Determines beggining of file transfer
  if (send_control_packet(fd, START_C, filename, length) < 0) {
    return -1;  // Error processing occurs here (if any)
  }

  // Transfer file data (Considering the Max Data Bytes per Packet defined in the header)
  unsigned int sent_bytes = 0, packet_i = 0;

  while (sent_bytes + MAX_DATA_LEN <= length) {
    int packet_status = send_data_packet(fd, packet_i++, file_content + sent_bytes, MAX_DATA_LEN);

    if (packet_status < 0) { // Error Occured (Any processing of the error occurs within this if)
      return packet_status;
    }

    sent_bytes += MAX_DATA_LEN;
  }

  int packet_status = send_data_packet(fd, packet_i++, file_content + sent_bytes, length - sent_bytes);

  if (packet_status < 0) {
    return packet_status;
  }

  // Determines ending of file transfer
  if (send_control_packet(fd, END_C, filename, length) < 0) {
    return -1;  // Error processing occurs here (if any)
  }

  // Terminates connection
  llclose(fd);

	return packet_i; // Return number of packets sent
}

int receive_file(char * port) {
  // Local variables
  char filename[255];
  unsigned int file_length = 0;

  // Establish connection
  int fd = llopen(port, RECEIVER);

  // Determines beggining of file transfer
  if (receive_control_packet(fd, START_C, filename, &file_length) < 0) {
    return -1;  // Error processing occurs here (if any)
  }

  // Receives file
  char buffer[file_length];
  unsigned int received_bytes = 0, packet_i = 0;

  while (received_bytes < file_length) {
    int packet_bytes = 0;

    unsigned int packet_status = receive_data_packet(fd, buffer + received_bytes, &packet_bytes);

    if (packet_status < 0 || packet_status != packet_i++ % 256) {
      // Error processing occurs here (if any)
      return -1;
    }

    received_bytes += packet_bytes;
	  printf("Data packet received: %d bytes of %d.\n", received_bytes, file_length);
  }

  // Determines ending of file transfer
  if (receive_control_packet(fd, END_C, filename, &file_length) < 0) {
    return -1;  // Error processing occurs here (if any)
  }

  // Terminates connection
  llclose(fd);

  // Open file to be sent
  FILE* file = fopen(filename, "w");
  if (!file) {
    printf("ERROR: File: %s could not be opened.\n", filename);
    return -1;
  }

  // Write content to file
  for(int i = 0; i < file_length; i++) {
    fwrite(buffer+i, 1, 1, file);
  }

  return 0;
}
