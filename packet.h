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

typedef unsigned char p_id;

#define PID_LOGIN 0x01
typedef struct {
	p_id packet_id;
	int32_t entity_id;
	int64_t world_seed;
	char dimension;
} StoC_Login;

#define PID_HANDSHAKE 0x02
typedef struct {
	p_id packet_id;
	char *connection_hash;
} StoC_Handshake;
typedef struct {
	p_id packet_id;
	char *username;
} CtoS_Handshake;

void send_packet(int fd, void *packet);
void *parse_packet(int fd);
// free_packet

#endif