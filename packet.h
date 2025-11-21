#ifndef PACKET_H
#define PACKET_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>

#include <arpa/inet.h>

#include "packet.h"

typedef unsigned char p_id;

#define PID_LOGIN 0x01
#define PID_HANDSHAKE 0x02
#define PID_PLAYER_POS_AND_LOOK 0x0D

typedef struct {

	p_id id;

	char int8s[1];
	int32_t int32s[1];
	int64_t int64s[1];
	char strings[1][256];

} Packet; // basically the union of all packets' possible fields

void send_packet(int fd, const Packet *packet);
void parse_packet(int fd, Packet *packet_out);

#endif