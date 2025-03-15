#ifndef __YAR_TOKEN_H
#define __YAR_TOKEN_H

#include "yar_literal.h"
#include "data_structure/list_head.h"
#include "data_structure/string.h"

typedef enum {

    IDENTIFIER, NUMBER, STRING,

    IF, ELSE, FOR, WHILE,
    LOCAL, EXPORT,
    AND, OR, TRUE, FALSE, NIL,
    FUN, RETURN,

    EQUAL, EQUAL_EQUAL,
    LESS, LESS_EQUAL,
    GREATER, GREATER_EQUAL,
    BANG, BANG_EQUAL,

    SUBSTITUTION_BEGIN,
    ARITHMETIC_BEGIN, DOUBLE_RIGHT_PARENTHESIS,
    SUBCOMMAND_BEGIN,

    LEFT_PARENTHESIS, RIGHT_PARENTHESIS,
    LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, DOLLAR,
    PLUS, MINUS, STAR, SLASH,
    SEMICOLON, COLON, QUESTION_MARK,

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

#endif

