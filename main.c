#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h>

void send_packet(int fd, unsigned char packet_id, char *data, size_t data_len) {

	write(fd, &packet_id, 1);
	write(fd, data, data_len);
}

uint16_t parse_string16(int fd, char *out) {

	uint16_t length;
	read(fd, &length, 2);
	length = __builtin_bswap16(length);

	for (int i = 0; i < length; i++) {

		read(fd, out + i, 1); // have to discard the first byte somehow
		read(fd, out + i, 1);
	}

	out[length] = '\0';

	return length + 1;
}

// reads and returns exactly one packet (I'll probably separate this into a separate file later)
void *parse_packet(int fd) {
	
	unsigned char packet_id;
	read(fd, &packet_id, 1);

	char buf[128];
	uint16_t buf_len;

	switch (packet_id) {

		case 0x02: // Handshake
			buf_len = parse_string16(fd, buf);
	}

	void *out = malloc(buf_len);
	memcpy(out, buf, buf_len);

	return out;
}

void client(int fd) {

	// Handshake
	const char *username = parse_packet(fd);

	printf("Client %s connected.\n", username);

	send_packet(fd, 2, "\0\1\0-", 4); // UTF16 string prefixed with its length (2 bytes)

	// Login

	// disconnect
	printf("Client %s disconnected.\n", username);

	free(username);
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
