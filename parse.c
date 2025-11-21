#include "packet.h"

// parses a UTF16-encoded string (prefixed with its length, 2 bytes) into an ASCII-encoded string (null-terminated)
// returns length of string in BYTES
static uint16_t parse_string16(int fd, char *out) {

	uint16_t length;
	read(fd, &length, 2);
	length = __builtin_bswap16(length);

	if (length > MAX_STRING_LENGTH_CHARS) { // safeguard
		out[0] = '\0';
		return 1;
	}

	for (int i = 0; i < length; i++) {

		read(fd, out + i, 1); // have to discard the first byte somehow
		read(fd, out + i, 1);
	}

	out[length] = '\0';

	return length + 1;
}

static void parse_int64(int fd, int64_t *out) {
	
	read(fd, out, 8);
	*out = __builtin_bswap64(*out);
}

static void parse_int32(int fd, int32_t *out) {
	
	read(fd, out, 4);
	*out = __builtin_bswap32(*out);
}

static void parse_int16(int fd, int16_t *out) {
	
	read(fd, out, 2);
	*out = __builtin_bswap16(*out);
}

static void parse_int8(int fd, int8_t *out) {
	
	read(fd, out, 1);
}

// reads and returns exactly one packet
void parse_packet(int fd, Packet *packet_out) {
	
	p_id packet_id;
	read(fd, &packet_id, 1);

	packet_out->id = packet_id;

	switch (packet_id) {

		case PID_LOGIN:
			parse_int32(fd, &packet_out->int32s[0]); 		// protocol version
			parse_string16(fd, packet_out->strings[0]); 	// username
			parse_int64(fd, &packet_out->int64s[0]); 		// unused
			parse_int8(fd, &packet_out->int8s[0]); 			// unused
			break;

		case PID_HANDSHAKE:
			parse_string16(fd, packet_out->strings[0]); 	// username
			break;

		default:
			printf("Packet ID 0x%02x not configured for parsing! Future packets will be malformed!\n", packet_id);
			break;
	}
}