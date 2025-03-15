#include "yar_literal.h"

const literal null_literal = { NONE, NULL };

void free_literal (literal lit) {
    if (lit.object == NULL)
        return;
    // simple case we can just free the pointer
    // complex case we must free further...
    free (lit.object);
}

