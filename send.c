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

static inline void send_double(int fd, double msg) {

	msg = __builtin_bswap64(msg);
	write(fd, &msg, 8);
}

static inline void send_float(int fd, float msg) {

	msg = __builtin_bswap32(msg);
	write(fd, &msg, 4);
}

static inline void send_int64(int fd, int64_t msg) {

	msg = __builtin_bswap64(msg);
	write(fd, &msg, 8);
}

static inline void send_int32(int fd, int32_t msg) {

	msg = __builtin_bswap32(msg);
	write(fd, &msg, 4);
}

static inline void send_int16(int fd, int16_t msg) {

	msg = __builtin_bswap16(msg);
	write(fd, &msg, 2);
}

static inline void send_int8(int fd, int8_t msg) {

	write(fd, &msg, 1);
}

void send_packet(int fd, const Packet *packet) {

	// write header (packet ID)
	write(fd, &packet->id, 1);

	// write packet body based on packet ID
	switch (packet->id) {

		case PID_LOGIN:
			send_int32(fd, packet->int32s[0]); 		// entity id // might have to flip endianness of numericals idc rn
			send_string16(fd, "");
			send_int64(fd, packet->int64s[0]); 		// world seed
			send_int8(fd, packet->int8s[0]);	 	// dimension
			break;

		case PID_HANDSHAKE:
			send_string16(fd, "-");
			break;

		case PID_SET_HEALTH:
			send_int16(fd, packet->int16s[0]);
			break;
		
		case PID_PLAYER_POS_AND_LOOK:
			send_double(fd, packet->doubles[0]); // X
			send_double(fd, packet->doubles[1]); // Y
			send_double(fd, packet->doubles[2]); // stance
			send_double(fd, packet->doubles[3]); // Z
			send_float(fd, packet->floats[0]); // yaw
			send_float(fd, packet->floats[1]); // pitch
			send_int8(fd, packet->int8s[0]); // on ground (unused?)
			break;
		
		default:
			printf("Packet ID 0x%02x not configured for sending!\n", *(p_id *) packet);
			break;
	}
}