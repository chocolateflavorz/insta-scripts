#ifndef REMOTE_LIB_LI5660_H
#define REMOTE_LIB_LI5660_H 1

#define IP_ADDR_LI5660_DOWN "192.168.3.69"
#define IP_ADDR_LI5660_UP "192.168.3.131"
#define PORT_LI5660 (5025)

bool li5660_set_format(int socket);
bool li5660_check_format(int socket);

// assuming: FORM ASC; DATA 7
bool li5660_fetch (int socket, double* r, double* theta);

#endif
