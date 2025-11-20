#ifndef PACKET_H
#define PACKET_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

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
	p_id packet_id; // since connection hash is always "-", there's no field for it here
} StoC_Handshake;
typedef struct {
	p_id packet_id;
	char username[16];
} CtoS_Handshake;

#define PID_PLAYER_POS_AND_LOOK 0x0D
typedef struct {
	p_id packet_id; // no fields yet because I just want this to work and floats are weird
} CtoS_PlayerPosAndLook;

void send_packet(int fd, void *packet);
void *parse_packet(int fd);
void free_packet(void *packet);

#endif