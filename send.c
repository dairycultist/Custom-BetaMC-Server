#include "packet.h"

// takes in an ASCII-encoded string and sends it as a UTF16-encoded string
static void send_string16(int fd, char *msg) {

	static const char zero[1] = {0};

	uint16_t length = strlen(msg);

	length = __builtin_bswap16(length); // I think this depends on the endianness of the system, but idc
	write(fd, &length, 2);
	length = __builtin_bswap16(length);
	
	for (int i = 0; i < length; i++) {

		write(fd, zero, 1);
		write(fd, msg + i, 1);
	}
}

void send_packet(int fd, void *packet) {

	// write header (packet ID)
	write(fd, (p_id *) packet, 1);

	// write packet body based on packet ID
	switch (*(p_id *) packet) {

		case PID_LOGIN:
			write(fd, &((StoC_Login *) packet)->entity_id, 4); // might have to flip endianness of numericals idc rn
			send_string16(fd, "");
			write(fd, &((StoC_Login *) packet)->world_seed, 8);
			write(fd, &((StoC_Login *) packet)->dimension, 1);
			break;

		case PID_HANDSHAKE:
			send_string16(fd, ((StoC_Handshake *) packet)->connection_hash);
			break;
	}
}