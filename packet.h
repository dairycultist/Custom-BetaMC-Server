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

#define MAX_STRING_LENGTH_CHARS 255

typedef unsigned char p_id;

#define PID_LOGIN 0x01
#define PID_HANDSHAKE 0x02
#define PID_TIME 0x04
#define PID_SET_HEALTH 0x08
#define PID_PLAYER_POS_AND_LOOK 0x0D

typedef struct {

	p_id id;

	int8_t int8s[1]; // also act as bools
	int16_t int16s[1];
	int32_t int32s[1];
	int64_t int64s[1];
	float floats[2]; // 32-bit
	double doubles[4]; // 64-bit
	char strings[1][MAX_STRING_LENGTH_CHARS + 1];

} Packet; // basically the union of all packets' possible fields

void send_packet(int fd, const Packet *packet);
void parse_packet(int fd, Packet *packet_out);

#endif