#ifndef __YAR_INTERPRETER_H
#define __YAR_INTERPRETER_H

#include "yar_ast.h"
#include "yar_job.h"
#include <stdint.h>

extern int runtime_error;
extern int syntax_error;

void init_lexer ();
void interpret(const char *source);

#define SIM_STR_INT_BRACE                     (1ULL << 0)
#define SIM_STR_INT_DOUBLE_QUOTE              (1ULL << 1)
#define SIM_STR_INT_DO_GLOB                   (1ULL << 2)
string_list *interpret_string (int token, uint64_t flags, int *saved_token);

extern job *extracted_job;

#endif
