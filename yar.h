#ifndef __YAR_H
#define __YAR_H

#include "yar_token.h"

void runtime_error (char *message, token tok);

void report (int line, char *where, char *message);

void error (int line, char *message);

#endif

