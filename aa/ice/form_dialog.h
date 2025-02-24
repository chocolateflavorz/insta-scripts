

#ifndef FORMDIALOG_H
#define FORMDIALOG_H

#ifdef __cplusplus 
extern "C" {
#endif

// return the size of max size of buffer
int form_dialog(const char *title, const char *msg, char *buffer, int buffer_size);

#ifdef __cplusplus
}
#endif

#endif