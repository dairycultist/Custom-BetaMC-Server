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

void send_packet(int fd, const Packet *packet) {

	// write header (packet ID)
	write(fd, &packet->id, 1);

	// write packet body based on packet ID
	switch (packet->id) {

		case PID_LOGIN:
			write(fd, &packet->int32s[0], 4); // entity id // might have to flip endianness of numericals idc rn
			send_string16(fd, "");
			write(fd, &packet->int64s[0], 8); // world seed
			write(fd, &packet->int8s[0], 1); // dimension
			break;

		case PID_HANDSHAKE:
			send_string16(fd, "-");
			break;
		
		case PID_PLAYER_POS_AND_LOOK:
			write(fd, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 41);
			break;
		
		default:
			printf("Packet ID 0x%02x not configured for sending!\n", *(p_id *) packet);
			break;
	}
}