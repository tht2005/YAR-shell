#ifndef __YAR_TOKEN_H
#define __YAR_TOKEN_H

#include "yar_literal.h"
#include "data_structure/list_head.h"
#include "data_structure/string.h"

typedef enum {

    ALNUM, CHARACTER,

    IF, ELSE, FOR, WHILE,
    BREAK, CONTINUE,
    LOCAL, EXPORT,
    AND, OR,
    FUN, EXIT,

    EQUAL, EQUAL_EQUAL,
    LESS, LESS_EQUAL,
    GREATER, GREATER_EQUAL,
    BANG, BANG_EQUAL,

    GET_VAR,
    SUBSTITUTION_BEGIN,
    ARITHMETIC_BEGIN, DOUBLE_RIGHT_PARENTHESIS,
    SUBCOMMAND_BEGIN,

    LEFT_PARENTHESIS, RIGHT_PARENTHESIS,
    LEFT_BRACE, RIGHT_BRACE,
    PLUS, MINUS, STAR, SLASH,
    BACKSLASH, SEMICOLON,

    SINGLE_QUOTE, DOUBLE_QUOTE,

    END_LINE,

    END_OF_FILE,

    // builtin-commands
    // PRINT,

} token_type;

typedef struct {
    token_type type;
    string lexeme;
    literal lit;
    int line;
} token;

token new_token (token_type type, string lexeme, literal lit, int line);
void free_token (token tok);

#endif

