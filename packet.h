#ifndef PACKET_H
#define PACKET_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h>

#include "packet.h"

#define PID_HANDSHAKE 0x02

typedef struct {

	unsigned char packet_id;
	char *username;

} CtoS_Handshake;

void send_packet(int fd, unsigned char packet_id, char *data, size_t data_len);
void *parse_packet(int fd);
// free_packet

#endif