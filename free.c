#include "packet.h"

void free_packet(void *packet) {

	switch (*(p_id *) packet) {

		default:
			free(packet); // most packets don't have fields with additional allocations
	}
}