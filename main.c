#include "packet.h"

typedef struct {

	int fd;
	CtoS_Handshake *handshake; // idc rn

} Client;

Client *init_client(int fd) {

	// ensure client file descriptor is blocking (unlike server)
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) & ~O_NONBLOCK);

	// create Client object
	Client *client = malloc(sizeof(Client));
	client->fd = fd;

	// Handshake
	client->handshake = parse_packet(fd);

	printf("Client %s connected.\n", client->handshake->username);

	{
		StoC_Handshake ret = { PID_HANDSHAKE };
		send_packet(fd, &ret);
	}

	// Login
	{
		StoC_Login ret = { PID_LOGIN, 0, 0, 0 };
		send_packet(fd, &ret);
	}

	// Initialize player
	{
		CtoS_PlayerPosAndLook ret = { PID_PLAYER_POS_AND_LOOK };
		send_packet(fd, &ret);
	}

	return client;
}

void destroy_client(Client *client) {

	// disconnect
	// TODO in the final implementation, remove the client from the static client list
	close(client->fd);

	printf("Client %s disconnected.\n", client->handshake->username);

	free_packet(client->handshake);
}

int main() {

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

		if (client_fd != -1) {

			Client *client = init_client(client_fd);

			// TODO add to list of clients
		}

		// process all connected clients

		// destroy_client(client);
	}

	// shutdown(server_fd, SHUT_RDWR);
	// close(server_fd);

	// return 0;
}
