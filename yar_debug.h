#ifndef __YAR_DEBUG_H
#define __YAR_DEBUG_H

#include "yar_ast.h"

#ifdef __DEBUG
#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG_PRINT(...) ((void)0)
#endif

void debug_string_fragment_list (string_fragment_list *list);

#endif
