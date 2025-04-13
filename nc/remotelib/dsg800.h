#ifndef REMOTE_LIB_DSG800_H
#define REMOTE_LIB_DSG800_H 1

#define IP_ADDR_DSG836A "192.168.3.66"
#define PORT_DSG836A (5555)

bool dsg800_reset     (int socket);

// discard the first three decimal place.
bool dsg800_level     (int socket, double level_dBm);
// discard the first three decimal place.
bool dsg800_frequency (int socket, double freq_Hz);

bool dsg800_output_on (int socket);
bool dsg800_output_off(int socket);

#endif
