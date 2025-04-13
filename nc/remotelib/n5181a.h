#ifndef REMOTE_LIB_N5181A
#define REMOTE_LIB_N5181A 1

#define IP_ADDR_N5181A "192.168.3.65"
#define PORT_N5181A (5025)

bool n5181a_output_on(int socket);
bool n5181a_output_off(int socket);
bool n5181a_frequency(int socket, double freq);

#endif
