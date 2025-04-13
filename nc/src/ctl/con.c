#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

#include "./../../headers/con.h"
#include "./../../headers/commandq.h"

#define RECEIVE_BUFFER_SIZE 512
// #define COMMAND_BUFFER_SIZE 64  defined in commandq.h

static struct timespec wait_time = {0, 300000000}; // 300 ms
static void sleep_for_device()
{
    nanosleep(&wait_time, NULL);
}

int open_con(Con *c, const char *addr, int port, char delimiter, int waitsec)
{

    int s;
    int fs;
    struct sockaddr_in address;
    struct timeval tv;
    fd_set fdset;
    socklen_t optlen;

    if ((s = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        return -1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(addr);
    address.sin_port = htons(port);
    if ((fs = fcntl(s, F_GETFL, NULL)) == -1)
    {
        return -1;
    }
    fs |= O_NONBLOCK;
    if (fcntl(s, F_SETFL, fs) == -1)
    {
        return -1;
    }

    int r = connect(s, (struct sockaddr *)&address, sizeof(address));
    if (r < 0)
    {
        if (errno == EINPROGRESS)
        {
            do
            {
                tv.tv_sec = waitsec;
                tv.tv_usec = 0;
                FD_ZERO(&fdset);
                FD_SET(s, &fdset);
                r = select(s + 1, NULL, &fdset, NULL, &tv);
                if (r == -1 && errno != EINTR)
                {
                    return -1;
                }
                else if (r > 0)
                {
                    int valopt;
                    optlen = sizeof(int);
                    if (getsockopt(s, SOL_SOCKET, SO_ERROR, (void *)(&valopt), &optlen) == -1)
                    {
                        return -1;
                    }
                    if (valopt)
                    {
                        return -1;
                    }
                    break;
                }
                else
                {
                    // timeout
                    return -1;
                }
                sleep_for_device();
            } while (1);
        }
        else
        {
            return -1;
        }
    }
    if ((fs = fcntl(s, F_GETFL, NULL)) == -1)
    {
        return -1;
    }
    fs &= (~O_NONBLOCK);
    if (fcntl(s, F_SETFL, fs) == -1)
    {
        return -1;
    }
    c->socket = s;
    c->receive_buffer = malloc(RECEIVE_BUFFER_SIZE);
    c->command_buffer = malloc(COMMAND_BUFFER_SIZE);
    c->delimiter = delimiter;
    return 0;
}
/*
*  For Blocking mode
int open_con(Con *c, const char *addr, int port, char delimiter)
{

    int r;
    unsigned address_size;
    struct sockaddr_in address;

    address_size = sizeof(address);
    if ((r = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        return -1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(addr);
    address.sin_port = htons(port);

    if (connect(r, (struct sockaddr *)&address, address_size) != 0)
    {
        return -1;
    }
    c->socket = r;
    c->receive_buffer = malloc(RECEIVE_BUFFER_SIZE);
    c->command_buffer = malloc(COMMAND_BUFFER_SIZE);
    c->delimiter = delimiter;
    return r;
}
*/
/*
int set_command(Con *c, const char *command)
{
    int len;

    len = strnlen(command, COMMAND_BUFFER_SIZE - 2);
    if (len == COMMAND_BUFFER_SIZE - 2)
    {
        return -1;
    }
    strncpy(c->command_buffer, command, len);
    c->command_buffer[len] = c->delimiter;
    c->command_buffer[len + 1] = 0x00;
    return len+1;
}
*/

int send_command(Con *c, const char *command)
{
    int len;
    len = strnlen(command, COMMAND_BUFFER_SIZE - 2);
    if (len == COMMAND_BUFFER_SIZE - 2)
    {
        return -1;
    }
    strncpy(c->command_buffer, command, len);
    c->command_buffer[len] = c->delimiter;
    c->command_buffer[len + 1] = 0x00; // not necessary
    len = send(c->socket, c->command_buffer, len + 1, 0);
    sleep_for_device();
    return len;
}

int query_command(Con *c, const char *command)
{
    int len;
    len = send_command(c, command);
    if (len == -1)
    {
        return -1;
    }
    len = recv(c->socket, c->receive_buffer, RECEIVE_BUFFER_SIZE - 1, 0);
    c->receive_buffer[len] = 0x00;
    return len;
}

int query_until(Con *c, const char *command, char recv_delimiter)
{
    int len = 0;
    char recv_byte;
    if (send_command(c, command) == -1)
    {
        return -1;
    }

    do
    {
        recv(c->socket, &recv_byte, 1, 0);
        c->receive_buffer[len++] = recv_byte;
    } while (recv_byte != recv_delimiter || len >= RECEIVE_BUFFER_SIZE - 1);
    c->receive_buffer[len] = 0x00;
    return len;
}

void close_con(Con *c)
{
    if (c->receive_buffer != NULL)
    {
        free(c->receive_buffer);
    }
    if (c->command_buffer != NULL)
    {
        free(c->command_buffer);
    }
    if (c->socket != -1)
    {
        shutdown(c->socket, SHUT_RDWR);
        close(c->socket);
    }
    return;
}
