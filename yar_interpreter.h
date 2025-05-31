#ifndef __YAR_INTERPRETER_H
#define __YAR_INTERPRETER_H

#include "data_structure/string.h"

void init_lexer ();
void interpret(const char *source);

#define SIM_STR_INT_BRACE                     (1 << 0)
#define SIM_STR_INT_DOUBLE_QUOTE              (1 << 1)
string simple_string_interpret (int flags);

#endif
