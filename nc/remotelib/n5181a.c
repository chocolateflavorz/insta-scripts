#include <stdio.h>
#include <stdbool.h>
#include "remote.h"

static char n5181a_buf[64];
bool n5181a_output_on(int socket) {
	send_to_equipment(socket, "OUTP ON");
	return true;
}

bool n5181a_output_off(int socket) {
  send_to_equipment(socket, "OUTP OFF");
  return true;
}


bool n5181a_frequency(int socket, double freq) {
	sprintf(n5181a_buf, "SOUR:FREQ %e", freq);
	send_to_equipment(socket, n5181a_buf);
	return true;
}
