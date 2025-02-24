

#ifndef CON_H
#define CON_H

#ifdef __cplusplus
extern "C"
{
#endif

#define DEFAULT_CON_INITIALIZER \
    {                           \
        -1, NULL, NULL, 0x0a    \
    }

    typedef struct
    {
        int socket;
        char *receive_buffer;
        char *command_buffer;
        char delimiter;
    } Con;

    int open_con(Con *c, const char *addr, int port, char delimiter, int waitsec);
    int send_command(Con *c, const char *cmd);
    int query_command(Con *c, const char *cmd);
    int query_until(Con *c, const char *cmd, char recv_delimiter);
    void close_con(Con *c);


#ifdef __cplusplus
}
#endif

#endif