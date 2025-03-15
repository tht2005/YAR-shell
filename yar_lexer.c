#include "yar_lexer.h"
#include "data_structure/list_head.h"
#include "data_structure/string.h"

char *source;
int length;
int start;
int current;
int line;
list_head head;

int isAtEnd () {
    return current >= length;
}
char advance () {
    return source[current++];
}
char peek () {
    if (isAtEnd ()) return '\0';
    return source[current];
}
char peekNext () {
    if (current + 1 >= length) return '\0';
    return source[current + 1];
}
int match (char expected) {
    if (isAtEnd ()) return 0;
    if (source[current] != expected) return 0;
    ++current;
    return 1;
}

inline int is_numeric (char c) {
    return 0 <= 'c' && c <= '9';
}
inline int is_alpha (char c) {
    return ('a' <= c && c <= 'z')
        || ('A' <= c && c <= 'Z')
        || c == '_';
}

void add_token4 (token_type type, string lexeme, literal lit, int line) {
    token_list *ptr = (token_list *)malloc (sizeof (token_list));
    if (ptr == NULL) {
        fprintf (stderr, "Unable to memory alloc a token_list.\n");
        exit (1);
    }

    ptr->tok = new_token(type, lexeme, lit, line);
    list_add_back (&ptr->link_node, &head);
}

void add_token (token_type type) {

    add_token4 (type, NULL, null_literal, line);
}

void scan_token () {

    char c = advance ();
    switch (c) {
        case ' ':
        case '\r':
        case '\t':
            break;

        case '\n':
            ++line;
            add_token (END_LINE);
            break;

        case '(':
            add_token (LEFT_PARENTHESIS);
            break;
        case ')':
            add_token (match(')') ? DOUBLE_RIGHT_PARENTHESIS : RIGHT_PARENTHESIS);
            break;
        case '{':
            add_token (LEFT_BRACE);
            break;
        case '}':
            add_token (RIGHT_BRACE);
            break;

        case '$':
            if (match ('(')) {
                if (match ('(')) add_token (ARITHMETIC_BEGIN);
                else add_token (SUBCOMMAND_BEGIN);
            }
            else if (match ('{'))
                add_token (SUBSTITUTION_BEGIN);
            else
                add_token (DOLLAR);
            break;

        case ',':
            add_token (COMMA);
            break;
        case '.':
            add_token (DOT);
            break;
        case '?':
            add_token (QUESTION_MARK);
            break;
        case ':':
            add_token (COLON);
            break;
        case ';':
            add_token (SEMICOLON);
            break;

        case '=':
            add_token (match('=') ? EQUAL_EQUAL : EQUAL);
            break;
        case '!':
            add_token (match('=') ? BANG_EQUAL : BANG);
            break;
        case '<':
            add_token (match('=') ? LESS_EQUAL : LESS);
            break;
        case '>':
            add_token (match('=') ? GREATER_EQUAL : GREATER);

        default:
            break;
    }

}

list_head *scan_tokens(char *str) {
    source = str;
    length = strlen (source);
    start = current = 0;
    line = 1;
    init_list_head(&head);

    while (!isAtEnd ()) {
        start = current;
        scan_token ();
    }
    add_token (END_OF_FILE);

    return &head;
}

