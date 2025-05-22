#include "yar_error.h"

#include <stdio.h>

int had_error = 0;
int had_runtime_error = 0;

int get_had_error () {
    return had_error;
}
int get_had_run_time_error () {
    return had_runtime_error;
}

// void runtime_error (char *message) {
//     hadRuntimeError = 1;
// }

void report (int line, char *where, char *message) {
    fprintf (stderr, "[line %d] Error %s: %s\n", line, where, message);
    had_error = 1;
}
void error (int line, char *message) {
    report (line, "", message);
}

void reset_error_status () {
    had_error = 0;
    had_runtime_error = 0;
}

