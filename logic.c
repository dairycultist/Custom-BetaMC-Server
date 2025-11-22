#include "packet.h"
#include "logic.h"

// all these functions return 0 if everything is good, otherwise return non-zero to indicate the client should be removed from the client list

int init_client(Client *client) {

	Packet packet;

	// parse incoming handshake
	parse_packet(client->fd, &packet);
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

	// parse incoming login packet (nothing we need is in it, so we just ignore the contents)
	parse_packet(client->fd, &packet);

	return 0;
}

int process_client_packet(Client *client, const Packet *packet) { // TODO take in array list of clients

	// use packet ID!

	return 0;
}

int process_loop() { // TODO take in array list of clients

	return 0;
}