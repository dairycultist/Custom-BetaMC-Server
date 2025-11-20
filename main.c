#include "packet.h"

typedef struct {

	int fd;
	CtoS_Handshake *handshake; // idc rn

} Client;

// temp maybe
#define MAX_PLAYER_COUNT 100

Client *clients[MAX_PLAYER_COUNT];
struct pollfd fds[MAX_PLAYER_COUNT];

void add_client(int fd) {

	// ensure client file descriptor is blocking (unlike server)
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) & ~O_NONBLOCK);

	// create Client object
	Client *client = malloc(sizeof(Client));
	client->fd = fd;

	client->handshake = parse_packet(fd);

	printf("Client %s connected.\n", client->handshake->username);

	// initialize Client/player
	StoC_Handshake handshake = { PID_HANDSHAKE };
	send_packet(fd, &handshake);

	StoC_Login login = { PID_LOGIN, 0, 0, 0 };
	send_packet(fd, &login);

	CtoS_PlayerPosAndLook pal = { PID_PLAYER_POS_AND_LOOK };
	send_packet(fd, &pal);

	// find a space to insert the client
	for (int i = 0; i < MAX_PLAYER_COUNT; i++) {

		if (clients[i] == NULL) {

			clients[i]    = client;
			fds[i].fd     = fd;
			break;
		}
	}
}

void remove_client(int index) {

	close(clients[index]->fd);

	printf("Client %s disconnected.\n", clients[index]->handshake->username);

	free_packet(clients[index]->handshake);
	free(clients[index]);

	clients[index] = NULL;
	fds[index].fd  = -1;
}

int main() {

	for (int i = 0; i < MAX_PLAYER_COUNT; i++) {

		fds[i].fd     = -1;
		fds[i].events = POLLIN;
	}

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

	// main loop
	while (1) {

		// accept any new clients
		int client_fd = accept(server_fd, NULL, NULL);

		if (client_fd != -1)
			add_client(client_fd);

		// process all connected clients
		while (poll(fds, MAX_PLAYER_COUNT, 0) != 0) {

			for (int i = 0; i < MAX_PLAYER_COUNT; i++) {

				if (fds[i].revents & POLLIN) {

					// data received
					parse_packet(clients[i]->fd);
				}

				if (fds[i].revents & (POLLERR | POLLHUP)) {

					// socket was closed
					remove_client(i);
				}
			}
		}
	}

	// shutdown(server_fd, SHUT_RDWR);
	// close(server_fd);

	// return 0;
}
