#include <stdio.h>
#include "datalink.h"
#include "application.h"


int send_control_packet(int fd, int type, char * filename, unsigned int length) {
  unsigned int index = 0; //, attempts = 0;
  //int num_written_bytes = 0;
  char packet[CONTROL_PACKET_LEN + sizeof(filename) + sizeof(length)];

  if(sizeof(filename) > 255) {
      printf("Filename: %s too large to fit in packet.\n", filename);
      return -1;
  }

  packet[index++] = type;

  packet[index++] = FILESIZE_T;
  packet[index++] = sizeof(length);
  packet[index] = length;

  index += sizeof(length);

  packet[index++] = FILENAME_T;
  packet[index++] = sizeof(filename);
  for(int i = 0; i < sizeof(filename); i++) {
      packet[index++] = filename[i];
  }

  /*while (num_written_bytes < sizeof(packet) && attempts < MAX_ATTEMPTS) {
    int tmp_written_bytes = llwrite(fd, packet + num_written_bytes, sizeof(packet) - num_written_bytes);

    if(tmp_written_bytes > 0) {
      num_written_bytes += tmp_written_bytes;
    }
    else {
      attempts++;
    }
  }

  return num_written_bytes;
  */

  return llwrite(fd, packet, sizeof(packet));
}

int receive_control_packet(int fd, int type, char * filename, unsigned int * file_length) {
    int tmp_length = 0;
    unsigned int index = 0;
    char buffer[CONTROL_PACKET_LEN + 255 + sizeof(int)];

    int packet_length = llread(fd, buffer);

    if(type != buffer[index++]) {
      return -1;
    }

    while(index < packet_length) {

        switch (buffer[index++]) {
          case FILESIZE_T:
            tmp_length = buffer[index++];
            *file_length = buffer[index];
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

int send_data_packet(int fd, int N, char * buffer, unsigned int length) {
  unsigned int index = 0;
  char packet[DATA_PACKET_LEN + length];

  if((length/256) > 255) {
    printf("Length overflow\n");
    return -1;
  }

  packet[index++] = DATA_C;
  packet[index++] = N;
  packet[index++] = (char)(length/256);
  packet[index++] = (char)(length%256);

  for(int i = 0; i < length; i++) {
    packet[index++] = buffer[i];
  }

  return llwrite(fd, packet, sizeof(packet));
}


int receive_data_packet(int fd, char * buffer, int * buf_len) {
   unsigned int index = 0;
   char packet[DATA_PACKET_LEN + MAX_DATA_LEN];

   llread(fd, packet);

   if(packet[index++] != DATA_C) {
     printf("Wrong packet received.\n");
     return -1;
   }

   int N = packet[index++];

   *buf_len = 256*packet[index+1] + packet[index+ 2];
   index += 2;

   for(int i = 0; i < *buf_len; i++) {
     buffer[i] = packet[index++];
   }

   return N;
}
