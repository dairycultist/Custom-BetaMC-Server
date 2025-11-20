#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h>

void client(int fd) {

	printf("Client connected.\n");

	// TODO packet parser

	// const char *message = "GET /\r\n";
	// write(fd, message, strlen(message));

	char buffer[1024] = {0};
	
	while (read(fd, buffer, sizeof(buffer) - 1)) {
		printf(">%s\n", buffer);
		bzero(buffer, sizeof(buffer));
	}

	close(fd);

	printf("Client Disconnected.\n");
}

int main() {

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

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

	printf("Server initialized.\n");

	while (1) {

		int client_fd = accept(server_fd, NULL, NULL);

		pid_t pid = fork();

		if (pid < 0) {

			perror("Fork failed.\n");

		} else if (pid == 0) {

			client(client_fd); // getpid(), getppid()
			exit(0);
		}
	}

	// shutdown(server_fd, SHUT_RDWR);
	// close(server_fd);

	// return 0;
}
