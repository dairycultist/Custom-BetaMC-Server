#include "packet.h"
#include "logic.h"

// TODO replace with a linked list instead of a static array
#define MAX_PLAYER_COUNT 100

static Client *clients[MAX_PLAYER_COUNT];
static struct pollfd client_fds[MAX_PLAYER_COUNT];

void add_client(int fd) {

	// ensure client file descriptor is blocking (unlike server)
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) & ~O_NONBLOCK);

	// create Client object
	Client *client = malloc(sizeof(Client));
	client->fd = fd;

	// send all the relevant stuff to the client
	if (init_client(client) != 0) {

		printf("A client attempted to connect, but failed!\n");

		close(fd);
		free(client);

		return;
	}

	printf("Client %s connected.\n", client->username);

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

					// packet received
					parse_packet(clients[i]->fd, &cts_packet);

					process_client_packet(clients[i], &cts_packet);
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
		process_loop();
	}

	// remember to pthread_exit the client_processing_thread (although technically not necessary since it'll exit when we exit)
	// shutdown(server_fd, SHUT_RDWR);
	// close(server_fd);

	// return 0;
}
