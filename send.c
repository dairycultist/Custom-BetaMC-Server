#include "packet.h"

void send_packet(int fd, void *packet) {

	// write packet ID header
	write(fd, (p_id *) packet, 1);

	switch (*(p_id *) packet) {

		case PID_HANDSHAKE: {
			write(
				fd,
				((StoC_Handshake *) packet)->connection_hash,
				strlen(((StoC_Handshake *) packet)->connection_hash)
			);
			break;
		}
	}
}