#ifndef __YAR_LITERAL_H
#define __YAR_LITERAL_H

#include <stddef.h>
#include <stdlib.h>

typedef enum {
    INT, STR, BOOLEAN, NONE 
} literal_type;

typedef struct {

    literal_type type;
    void *object;
     
} literal;

extern const literal null_literal;

void free_literal (literal lit);

#endif

