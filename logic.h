#ifndef LOGIC_H
#define LOGIC_H

#include "packet.h"

typedef struct {

	int fd;
	char username[17];

} Client; // maybe make this the linked list node

int init_client(Client *client);
int process_client_packet(Client *client, const Packet *packet);
void process_loop();

#endif