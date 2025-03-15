#include "yar_token.h"

token new_token (token_type type, string lexeme, literal lit, int line)
{
    token tok;
    tok.type = type;
    tok.lexeme = lexeme;
    tok.lit = lit;
    tok.line = line;
    return tok;
}

void free_token (token tok) {
    free_string (tok.lexeme);
}

