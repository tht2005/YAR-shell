#ifndef __YAR_DEBUG_H
#define __YAR_DEBUG_H

#include "yar_ast.h"

#include <assert.h>

#ifdef __DEBUG
#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#define DEBUG_ASSERT(x) assert (x);
#else
#define DEBUG_PRINT(...) ((void)0)
#define DEBUG_ASSERT(x) ((void)0)
#endif

void debug_string_list (string_list *list);
void debug_string_fragment_list (string_fragment_list *list);

#endif
