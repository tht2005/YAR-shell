#ifndef __YAR_ERROR_H
#define __YAR_ERROR_H

int get_had_error ();

int get_had_run_time_error ();

// void runtime_error (char *message, token tok);

void report (int line, char *where, char *message);

void error (int line, char *message);

void reset_error_status ();

#endif
