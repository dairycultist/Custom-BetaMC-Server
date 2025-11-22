#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
// libdeflate.h

#include <arpa/inet.h>

#include "logic.h"

// a non-void function returns 0 if everything is good, otherwise returns
// non-zero to indicate the client should be removed from the client list
// (i.e. because it sent an invalid packet)

int init_client(Client *client) {

	Packet packet;

	// parse incoming handshake
	parse_packet(client->fd, &packet);

	if (packet.id != PID_HANDSHAKE)
		return 1;

	memcpy(client->username, packet.strings[0], 17);

	// send initialization packets
	packet.id = PID_HANDSHAKE;
	send_packet(client->fd, &packet);

	packet.id = PID_LOGIN;
	packet.int8s[0] = 0; // dimension
	packet.int32s[0] = 0; // entity id
	packet.int64s[0] = 0; // world seed
	send_packet(client->fd, &packet);

	packet.id = PID_SPAWN_POINT;
	packet.int32s[0] = 0; // X
	packet.int32s[1] = 64; // Y
	packet.int32s[2] = 0; // Z
	send_packet(client->fd, &packet);

	packet.id = PID_TIME;
	packet.int64s[0] = 18000; // time
	send_packet(client->fd, &packet);

	packet.id = PID_SET_HEALTH;
	packet.int16s[0] = 20; // health
	send_packet(client->fd, &packet);

	packet.id = PID_PLAYER_POS_AND_LOOK;
	packet.doubles[0] = 0; // X
	packet.doubles[1] = 64; // Y
	packet.doubles[2] = 1.62; // stance
	packet.doubles[3] = 0; // Z
	packet.floats[0] = 0; // yaw
	packet.floats[1] = 0; // pitch
	send_packet(client->fd, &packet);

	packet.id = PID_PRECHUNK;
	packet.int32s[0] = 0; // X
	packet.int32s[1] = 0; // Z
	packet.int8s[0] = 1; // load
	send_packet(client->fd, &packet);

	// parse incoming login packet
	parse_packet(client->fd, &packet);

	if (packet.id != PID_LOGIN)
		return 1;

	return 0;
}

int process_client_packet(Client *client, const Packet *packet) { // TODO take in array list of clients

	if (packet->id == PID_DISCONNECT)
		return 1;

	return 0;
}

void process_loop() { // TODO take in array list of clients

}