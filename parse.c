#include "packet.h"

static uint16_t parse_string16(int fd, char *out) { // UTF16-encoded string prefixed with its length (2 bytes)

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

// reads and returns exactly one packet
void *parse_packet(int fd) {
	
	p_id packet_id;
	read(fd, &packet_id, 1);

	switch (packet_id) {

		case PID_HANDSHAKE: { // Handshake

			CtoS_Handshake *out = malloc(sizeof(CtoS_Handshake));

			out->packet_id = PID_HANDSHAKE;
			out->username = malloc(128);

			parse_string16(fd, out->username);

			return out;
		}
	}

	return NULL;
}