#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h>

void send_packet(int fd, unsigned char packet_id, char *data, size_t data_len) {

	write(fd, &packet_id, 1);
	write(fd, data, data_len);
}

void client(int fd) {

	printf("Client connected.\n");

	send_packet(fd, 2, "\0\1\0-", 4); // UTF16 string prefixed with its length (2 bytes)

	// TODO packet parser
	char buffer[32] = {0};
	
	while (read(fd, buffer, sizeof(buffer) - 1)) {

		for (int i = 0; i < sizeof(buffer); i++)
			printf("%02x.", (unsigned int) buffer[i]);
		printf("\n");
		bzero(buffer, sizeof(buffer));
	}

	printf("Client disconnected.\n");
}

int main() {

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

	{
		struct sockaddr_in server_addr;

		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(25565);
		server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

		if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
			perror("Failed to bind.\n");
			exit(errno);
		}

		if (listen(server_fd, 99)) {
			perror("Failed to listen.\n");
			exit(errno);
		}
	}

	printf("Server initialized.\n");

	while (1) {

		int client_fd = accept(server_fd, NULL, NULL);

		pid_t pid = fork();

		if (pid < 0) {

			perror("Fork failed.\n");

		} else if (pid == 0) {

			client(client_fd); // getpid(), getppid()
			close(client_fd);
			exit(0);
		}
	}

	// shutdown(server_fd, SHUT_RDWR);
	// close(server_fd);

	// return 0;
}
