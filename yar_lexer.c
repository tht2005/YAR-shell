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

int is_numeric (char c) {
    return '0' <= c && c <= '9';
}
int is_alpha (char c) {
    return ('a' <= c && c <= 'z')
        || ('A' <= c && c <= 'Z')
        || c == '_';
}
int is_alpha_numeric (char c) {
    return is_alpha (c) || is_numeric (c);
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
    add_token4 (type, new_substr (source + start, source + current), null_literal, line);
}
void add_token2 (token_type type, literal lit) {
    add_token4 (type, new_substr (source + start, source + current), lit, line);
}

void scan_token () {
    char c = advance ();
    switch (c) {
        case '\r':
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
            else if ( is_alpha_numeric (peek ()) ) {
                while (!isAtEnd () && is_alpha_numeric (peek ()))
                    advance ();
                add_token (GET_VAR);
            }
            else {
                goto DFL_CASE;
            }
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
            break;
        case '+':
            add_token (PLUS);
            break;
        case '-':
            add_token (MINUS);
            break;
        case '*':
            add_token (STAR);
            break;
        case '/':
            // line comment
            if (match ('/')) {
                while (peek() != '\n' && !isAtEnd ())
                    advance ();
            }
            // multiline comment
            else if (match ('*')) {
                while ( !(peek () == '*' && peekNext() == '/') && !isAtEnd () )
                    advance ();
                if (isAtEnd ()) {
                    // raise error
                }
                advance ();
                advance ();
            }
            else
                add_token (SLASH);
            break;
        case '\\':
            add_token (BACKSLASH);
            break;
        case '\'':
            add_token (SINGLE_QUOTE);
            break;
        case '\"':
            add_token (DOUBLE_QUOTE);
            break;
        case '\n':
            ++line;
            add_token (END_LINE);
            break;
DFL_CASE:
        default:
            if (is_alpha_numeric (c)) {
                while (!isAtEnd() && is_alpha_numeric(peek())) {
                    advance ();
                }
                string s = new_substr (source + start, source + current);
                if (strcmp (s, "if") == 0)
                    add_token (IF);
                else if (strcmp (s, "else") == 0)
                    add_token (ELSE);
                else if (strcmp (s, "for") == 0)
                    add_token (FOR);
                else if (strcmp (s, "while") == 0)
                    add_token (WHILE);
                else if (strcmp (s, "local") == 0)
                    add_token (LOCAL);
                else if (strcmp (s, "export") == 0)
                    add_token (EXPORT);
                else if (strcmp (s, "break") == 0)
                    add_token (BREAK);
                else if (strcmp (s, "continue") == 0)
                    add_token (CONTINUE);
                else if (strcmp (s, "fun") == 0)
                    add_token (FUN);
                else if (strcmp (s, "exit") == 0)
                    add_token (EXIT);
                else
                    add_token (ALNUM);
                free_string (s);
            }
            else
                add_token (CHARACTER);
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
    start = current;
    add_token (END_OF_FILE);

    return &head;
}

void free_token_list (list_head *head) {
    list_head *p, *tmp;
    list_for_each_safe (p, tmp, head) {
        list_del (p);
        token_list *ptr = list_entry (p, token_list, link_node);
        free_token (ptr->tok);
        free (ptr);
    }
}

