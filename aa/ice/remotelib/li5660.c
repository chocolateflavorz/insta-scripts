#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "remote.h"

static char li5660_buf[64];
#define LI5660_BUFFER_LENGTH ((sizeof(li5660_buf) / sizeof(li5660_buf[0]))-1)

// :CALC1:FORM MLIN
// :CALC2:FORM PHASE
// :DATA 7
// :FORM ASC
// :FETC?
// STATUS, R, THETA

bool li5660_set_format(int socket) {
  send_to_equipment(socket, ":DATA 7");
  send_to_equipment(socket, ":FORM ASC");
  return true;
}

bool li5660_check_format(int socket) {
  // const char* expected_calc1_form = "MLIN";
  // const char* expected_calc2_form = "PHAS";
  const int   expected_data   = 7;
  const char* expected_format = "ASC";
  
  query_to_equipment(socket, ":DATA?", li5660_buf, LI5660_BUFFER_LENGTH, false);
  if (atoi(li5660_buf) != expected_data) {
    return false;
  }
  query_to_equipment(socket, ":FORM?", li5660_buf, LI5660_BUFFER_LENGTH, false);
  if (strcmp(li5660_buf, expected_format) != 0) {
    return false;
  }
  return true;
}

bool li5660_fetch (int socket, double* r, double* theta) {
  query_to_equipment(socket, ":FETC?", li5660_buf, LI5660_BUFFER_LENGTH, false);
  char* str = li5660_buf;
  char* delim = strchr(str, ',');
  if (delim == NULL) {
    return false;
  }
  *delim = 0x00;
  if (atoi(str) != 0) {
    // STATUS != 0, measurement error,
    return false;
  }
  str = delim + 1;
  delim = strchr(str, ',');
  if (delim == NULL) {
    return false;
  }
  *delim = 0x00;
  *r = atof(str);
  str = delim + 1;
  *theta = atof(str);
  return true;
}
