#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "remote.h"
#include "dsg800.h"

static char dsg800_buf[64];
#define DSG800_BUFFER_LENGTH ((sizeof(dsg800_buf) / sizeof(dsg800_buf[0]))-1)

bool dsg800_reset (int socket) {
  send_to_equipment(socket, ":SYST:PRES:TYPE FAC");
  send_to_equipment(socket, ":SYST:PRES");
  return true;
}

bool dsg800_level (int socket, double level_dBm) {
  if (level_dBm < -110 || level_dBm > 20) {
    return false;
  }
  int written_size = snprintf(dsg800_buf,
			      DSG800_BUFFER_LENGTH,
			      ":LEV %.2lf ",
			      level_dBm
			      );
  if (written_size >= DSG800_BUFFER_LENGTH || written_size < 0) {
    return false;
  }
  send_to_equipment(socket, dsg800_buf);
  return true;
}

bool dsg800_frequency (int socket, double freq_Hz) {
  if (freq_Hz < 9e3 || freq_Hz > 3e9) {
    return false;
  }
  int written_size = snprintf(dsg800_buf,
			      DSG800_BUFFER_LENGTH,
			      ":FREQ %e ",
			      freq_Hz
			      );
  if (written_size >= DSG800_BUFFER_LENGTH || written_size < 0) {
    return false;
  }
  send_to_equipment(socket, dsg800_buf);
  return true;
}

bool dsg800_output_on (int socket) {
  send_to_equipment(socket, ":OUTP ON");
  return true;
}

bool dsg800_output_off(int socket) {
  send_to_equipment(socket, ":OUTP OFF");
  return true;
}

void test_dsg800_lib() {
  int written_size = snprintf(dsg800_buf,
			      DSG800_BUFFER_LENGTH,
			      ":LEV %.2e Hz",
			      -103829.3734
			      );
   printf("input: -103829.3734Hz\n");
   printf("written_size: %d\n", written_size);
   printf("buf: %s", dsg800_buf);
 }



