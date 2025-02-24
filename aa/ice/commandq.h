

#ifndef COMMANDQ_H
#define COMMANDQ_H

#ifdef __cplusplus
extern "C"
{
#endif

#define COMMAND_BUFFER_SIZE 64
#define MAX_QUEUE_SIZE (16)

#define COMMANDQ_TYPE_SENDONLY  (1)
#define COMMANDQ_TYPE_SENDUNTIL (2)
#define COMMANDQ_TYPE_SENDRECV  (3)
#define COMMANDQ_TYPE_WAITTRIG  (7)

    typedef struct
    {
        char *queue;
        unsigned int index;
        char type_queue[MAX_QUEUE_SIZE];
    } CommandQ;

    void commandq_init(CommandQ *q);
    int commandq_push(CommandQ *q, const char *cmd, char type);
    int commandq_top(CommandQ *q, char *s, char *type);
    int commandq_pop(CommandQ *q, char *s, char *type);
    void commandq_reset(CommandQ *q);
    void commandq_free(CommandQ *q);

#ifdef __cplusplus
}
#endif

#endif