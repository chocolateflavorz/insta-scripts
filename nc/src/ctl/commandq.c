#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./../../headers/commandq.h"

void commandq_init(CommandQ *q)
{
    q->queue = malloc(COMMAND_BUFFER_SIZE * MAX_QUEUE_SIZE);
    q->index = 0;
}
int commandq_push(CommandQ *q, const char *cmd, char type)
{
    strncpy(&q->queue[q->index * COMMAND_BUFFER_SIZE], cmd, COMMAND_BUFFER_SIZE - 2);
    q->type_queue[q->index] = type;

    q->index++;
    if (q->index == MAX_QUEUE_SIZE)
    {
        return -1;
    }
    return q->index;
}

int commandq_top(CommandQ *q, char *s, char *type)
{
    if (q->index == 0)
    {
        s[0] = 0x00;
        *type = 0;
        return 0;
    }
    strncpy(s, &q->queue[(q->index-1) * COMMAND_BUFFER_SIZE], COMMAND_BUFFER_SIZE - 2);
    *type = q->type_queue[q->index-1];
    return q->index;
}
int commandq_pop(CommandQ *q, char *s, char *type)
{
    if (q->index == 0)
    {
        s[0] = 0x00;
        *type = 0;
        return q->index;
    }
    if (q->index == 0)
    {
        return -1;
    }
    q->index--;
    strncpy(s, &q->queue[q->index*COMMAND_BUFFER_SIZE], COMMAND_BUFFER_SIZE - 2);
    *type = q->type_queue[q->index];
    return q->index;
}

void commandq_reset(CommandQ *q)
{
    q->index = 0;
    return;
}

void commandq_free(CommandQ *q)
{
    free(q->queue);
}