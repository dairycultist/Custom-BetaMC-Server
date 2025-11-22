#ifndef LOGIC_H
#define LOGIC_H

typedef struct {

	int fd;
	char username[17];

} Client; // maybe make this the linked list node

int init_client(Client *client);
int process_client_packet(Client *client, const Packet *packet);
int process_loop();

#endif