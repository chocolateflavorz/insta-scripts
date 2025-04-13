

#ifndef MSGDIALOG_H
#define MSGDIALOG_H

#ifdef __cplusplus 
extern "C" {
#endif

typedef enum {
    YES = 1,
    NO = 0,
} YesNo;

YesNo yesno(const char *msg);

#ifdef __cplusplus
}
#endif

#endif