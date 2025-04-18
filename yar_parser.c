#include "yar_env.h"
#include "yar_parser.h"
#include "data_structure/string.h"

#include <ctype.h>
#include <string.h>

int source_len;
char *source;

int start;
int cur;
int inside_double_quote;

int skipped_char (char c) {
    return c == '\r';
}
int is_variable_char (char c) {
    return isalpha (c)
        || isdigit (c)
        || (c == '_');
}

int end () {
    return cur >= source_len;
}

int peek_char (int step) {
    if (cur + step >= source_len) {
        return 0;
    }
    return source[cur + step];
}
int cur_char () {
    return peek_char (0);
}
int next_char () {
    for (++cur; !end () && skipped_char (cur_char()); ++cur);
    return cur_char ();
}

void skip_space () {
    while (!end () && isspace (cur_char ()))
        ++cur;
}

void init_parser (char *s) {
    source = s;
    source_len = strlen (s);
    start = 0;
    cur = 0;
    inside_double_quote = 0;
}

string next_string (string);

string next_single_quote_string (string str) {
    ++cur;
    while (!end () && cur_char () != '\'') {
        str = string_push_back (str, source[cur]);
        ++cur;
    }

    if (cur == source_len) {
        // raise error string not end
        free_string (str);
    }
    return str;
}

string next_double_quote_string (string str) {
    if (inside_double_quote != 0) {
        fprintf (stderr, "Yar: contradiction: visit next_double_quote_string but inside_db_quote != 0\n");
        exit (EXIT_FAILURE);
    }

    ++cur;
    inside_double_quote = 1;
    str = next_string (str);
    if (inside_double_quote != 0) {
        fprintf (stderr, "Yar: double quote string don't end ...\n");
        exit (EXIT_FAILURE);
    }
    ++cur;
    str = next_string (str);
    return str;
}

string next_get_var (string str) {
    string var_name = new_string ();
    while (is_variable_char (cur_char())) {
        var_name = string_push_back (var_name, cur_char());
        ++cur;
    }
    char *value = yar_getenv(var_name);
    if (value == NULL) {
        fprintf (stderr, "Yar: next_get_var: yar_getenv return NULL!\n");
        exit (EXIT_FAILURE);
    }
    str = string_append_back(str, value);
    free_string (var_name);
    str = next_string (str);
    return str;
}

string next_get_substitution (string str) {
    // ??????
}

int check_assignment () {
    int i = 0;
    char c;
    for (; (c = peek_char(i)) != 0; ++i) {
        if (c == '=') {
            // =    : false
            // a=   : true
            return (i > 0);
        }
        if (isdigit (c) && i == 0) {
            return 0;
        }
        if (!is_variable_char (c)) {
            return 0;
        }
        if (isspace (c)) {
            return 0;
        }
    }
    return 0;
}
// void assignment () {
//     string var_name = new_string();
//     while (cur_char() != '=') {
//         var_name = string_push_back (var_name, cur_char());
//         ++cur;
//     }
//     string value = new_string();
//     if (!end () && !isspace (cur_char())) {
//         value = next_string (value);
//     }
//     yar_setenv (var_name, value, 1);
//     free_string (var_name);
//     free_string (value);
// }

string next_string (string str) {
    char c;
    int cont_retrieve = 1;

    skip_space ();

    for(; !end() && cont_retrieve; ++cur) {

        switch (c = cur_char ()) {

            case '$':
                c = next_char ();
                // change get_var condition??
                if (is_variable_char (c)) {
                    str = next_get_var (str);
                }
                else if (c == '{') {
                    str = next_get_substitution (str);
                }
                else if (c == '(') {
                    c = next_char ();
                    if (c == '(') {
                        // arithmetic
                    }
                    else {
                        // sub command
                    }
                }
                break;

            case '\'':
                if (inside_double_quote)
                    str = string_push_back (str, c);
                else
                    str = next_single_quote_string (str);
                break;

            case '\"':
                if (inside_double_quote) {
                    cont_retrieve = 0;
                    inside_double_quote = 0;
                }
                else {
                    str = next_double_quote_string (str);
                }
                break;

            case '\\':
                c = next_char ();
                switch (c) {
                    case '\n':
                        // \\\n for multiline command 
                        break;
                    case 0:
                        // throw error
                        break;
                    default:
                        str = string_push_back (str, c);
                        break;
                }
                break;

            case '#':
                if (inside_double_quote)
                    str = string_push_back (str, c);
                else {
                    // comment
                }
                break;

            case ' ':
            case '\n':
            //case ';':
                if (inside_double_quote)
                    str = string_push_back (str, c);
                else
                    cont_retrieve = 0;
                break;

            default:
                str = string_push_back (str, c);
                break;

        }
    }
    return str;
}

void command () {
    while (skip_space(), check_assignment ()) {
        string name = new_string ();
        while (cur_char () != '=') {
            name = string_push_back (name, cur_char());
            ++cur;
        }
        // skip '='
        ++cur;

        string value = new_string ();
        value = next_string (value);

        printf ("debug %s->%s\n", name, value);

        free_string (name);
        free_string (value);
    }

    int cont = 1;
    while (cont) {
        string str = new_string ();
        str = next_string (str);
        printf ("===%s===\n", str);
        if (string_length (str) == 0) {
            cont = 0;
        }
        free_string (str);
    }
}

void test () {
    command ();
}

