uint16_t parse_string16(int fd, char *out) { // UTF16-encoded string prefixed with its length (2 bytes)

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