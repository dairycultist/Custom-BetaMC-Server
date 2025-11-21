#include "packet.h"

typedef struct {

	int fd;
	char username[17];

} Client;

// temp maybe
#define MAX_PLAYER_COUNT 100

Client *clients[MAX_PLAYER_COUNT];
struct pollfd client_fds[MAX_PLAYER_COUNT];

void add_client(int fd) {

	Packet packet;

	// ensure client file descriptor is blocking (unlike server)
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) & ~O_NONBLOCK);

	// create Client object
	Client *client = malloc(sizeof(Client));
	client->fd = fd;

	// parse incoming handshake
	parse_packet(fd, &packet);
	memcpy(client->username, packet.strings[0], 17);

	// initialize Client/player
	packet.id = PID_HANDSHAKE;
	send_packet(fd, &packet);

	packet.id = PID_LOGIN;
	packet.int8s[0] = 0; // dimension
	packet.int32s[0] = 0; // entity id
	packet.int64s[0] = 0; // world seed
	send_packet(fd, &packet);

	packet.id = PID_PLAYER_POS_AND_LOOK;
	packet.doubles[0] = 0; // X
	packet.doubles[1] = 64; // Y
	packet.doubles[2] = 0; // stance
	packet.doubles[3] = 0; // Z
	packet.floats[0] = 0; // yaw
	packet.floats[1] = 0; // pitch
	send_packet(fd, &packet);

	// parse incoming login packet
	parse_packet(fd, &packet);

	printf("Client %s connected (protocol %d).\n", client->username, packet.int32s[0]);

	// find a space to insert the client
	for (int i = 0; i < MAX_PLAYER_COUNT; i++) {

		if (clients[i] == NULL) {

			clients[i]       = client;
			client_fds[i].fd = fd;
			break;
		}
	}
}

void remove_client(int index) {

	close(clients[index]->fd);

	printf("Client %s disconnected.\n", clients[index]->username);

	free(clients[index]);

	clients[index]        = NULL;
	client_fds[index].fd  = -1;
}

void *client_processing_thread_routine(void *server_fd) {

	// pthread_exit(), pthread_join(), pthread_mutex_t, pthread_mutex_lock(), pthread_mutex_unlock(), pthread_cond_t, pthread_cond_wait(), pthread_cond_signal()

	for (int i = 0; i < MAX_PLAYER_COUNT; i++) {

		client_fds[i].fd     = -1;
		client_fds[i].events = POLLIN;
	}

	Packet cts_packet;

	while (1) {

		// accept any new clients
		int client_fd = accept(*(int *) server_fd, NULL, NULL);

		if (client_fd != -1)
			add_client(client_fd);

		// process all connected clients
		if (poll(client_fds, MAX_PLAYER_COUNT, 0) != 0) {

			for (int i = 0; i < MAX_PLAYER_COUNT; i++) {

				if (client_fds[i].revents & POLLIN) {

					// data received
					parse_packet(clients[i]->fd, &cts_packet);

					// TODO do stuff with received packet based on id
				}

				if (client_fds[i].revents & (POLLERR | POLLHUP)) {

					// socket was closed
					remove_client(i);
				}
			}
		}
	}

	return NULL;
}

int main() {

	// create server socket
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

	// make socket non-blocking
	fcntl(server_fd, F_SETFL, fcntl(server_fd, F_GETFL, 0) | O_NONBLOCK);

	// initialize server information
	{
		struct sockaddr_in server_addr;

		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(25565);
		server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

		if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
			printf("Failed to bind.\n");
			exit(errno);
		}

		if (listen(server_fd, 99)) {
			printf("Failed to listen.\n");
			exit(errno);
		}
	}

	printf("Server initialized.\n");

	// create the client processing thread
	pthread_t client_processing_thread;

	if (pthread_create(&client_processing_thread, NULL, client_processing_thread_routine, &server_fd)) {

		printf("Failed to create client processing thread.\n");
		exit(1);
	}

	// main loop
	while (1) {

	}

	// remember to pthread_exit the client_processing_thread (although technically not necessary since it'll exit when we exit)
	// shutdown(server_fd, SHUT_RDWR);
	// close(server_fd);

	// return 0;
}
