#include "yar_interpreter.h"

#include "data_structure/string.h"
#include "yar_ast.h"
#include "yar_exec.h"
#include "yar_job.h"
#include "yar_parser.tab.h"
#include "yar_lexer.h"
#include "setjmp.h"

#include "libglob/glob.h"

#include <readline/readline.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define WHITESPACE_CHARACTERS       " \t\r"

job *extracted_job;

int check_redirection (int token)
{
    switch (token) {
        case LESS:
        case GREATER:
        case GREATER_DOUBLE:
        case AND_GREATER:
        case GREATER_AND:
        case AND_GREATER_DOUBLE:
        case LESS_AND:

        case NUM_LESS:
        case NUM_GREATER:
        case NUM_LESS_AND:
        case NUM_GREATER_AND:

            return 1;
        
        default:
            return 0;
    }
}

YYSTYPE yylval;

int runtime_error;
int syntax_error;
int token_next ()
{
    return yylex (&yylval);
}
int token_skip_space (int token)
{
    while (token == WHITESPACE)
    {
        token = token_next ();
    }
    return token;
}
int token_advance_skip_space (int token)
{
    while ((token = token_next()) == WHITESPACE)
    {
        // do nothing
    }
    return token;
}
int token_advance_skip_space_newline (int token)
{
    while ((token = token_next()), token == WHITESPACE || token == NEWLINE)
    {
        // do nothing
    }
    return token;
}

glob_t glob (const char *pattern)
{
    glob_t results;
    int ret = g_glob (pattern, GLOB_BRACE | GLOB_MARK | GLOB_NOCHECK | GLOB_TILDE, NULL, &results);
    return results;
}

char *strtok_r_respect_escape (char *restrict s, const char *restrict delim, char **restrict save_ptr)
{
    if (s == NULL) {
        if (*save_ptr == NULL) {
            return NULL;
        }
        s = (*save_ptr) + 1;
    }
    while (*s && (*s) != '\\' && strchr (delim, *s))
    {
        ++s;
    }
    if (*s == '\0') {
        return NULL;
    }
    *save_ptr = s;
    while (1) {
        if (**save_ptr == '\0') {
            *save_ptr = NULL;
            return s;
        }
        if (**save_ptr == '\\') {
            ++(*save_ptr);
            if (**save_ptr) ++ (*save_ptr);
            continue;
        }
        if (strchr (delim, **save_ptr)) {
            break;
        }
        ++(*save_ptr);
    }
    **save_ptr = '\0';
    return s;
}

string_list *split_word (const char *input)
{
    char *copy = strdup (input);
    char *saveptr;

    string_list *string_list_head = NULL, *string_list_tail = NULL;
    char *token = strtok_r_respect_escape (copy, WHITESPACE_CHARACTERS, &saveptr);
    while (token != NULL)
    {
        string new_str = new_string_2(token);
        string_list_push_back (&string_list_head, &string_list_tail, &new_str);
        token = strtok_r_respect_escape (NULL, WHITESPACE_CHARACTERS, &saveptr);
    }

    free (copy);
    return string_list_head;
}

string_list *interpret_string (int token, uint64_t flags, int *saved_token)
{
    string_fragment_list *string_fragment_head = NULL, *string_fragment_tail = NULL;
    string_fragment str_frag;

    // after that this behavior can be change base on flags
    // implement later ...
    if (token == TOK_NIL)
    {
        token = token_advance_skip_space_newline (token);
    }
    else {
        assert (token == STRING_LIST); 
    }

    // yypstate *parser = yypstate_new ();
    // int status;
    for(;; token = token_next())
    {
        if (token == STRING_LIST)
        {
            str_frag = yylval.str_frag;
            string_fragment_list_push_back (&string_fragment_head, &string_fragment_tail, &str_frag);
        }
        else if ((flags & SIM_STR_INT_DOUBLE_QUOTE) && token == DOUBLE_QUOTE) {
            break;
        }
        else if ((flags & SIM_STR_INT_BRACE) && token == BRACE_RIGHT) {
            break;
        }
        else {
            break;
        }
    }

    if (saved_token) {
        *saved_token = token;
    }

    string_list *string_list_head = NULL, *string_list_tail = NULL;
    string current_string = NULL;

    if (flags & SIM_STR_INT_BRACE) {
        current_string = string_push_back (current_string, '\{');
    }

    for (string_fragment_list *ptr = string_fragment_head; ptr; ptr = ptr->next)
    {
        string_list *head;
        switch (ptr->str_frag.type)
        {
            case STR_FRAG_QUOTED:
                // DEBUG_PRINT("debug: string_fragment quoted: `%s`\n", ptr->str_frag.value);
                current_string = string_append_back (current_string, ptr->str_frag.value);
                break;
            case STR_FRAG_NON_QUOTED:
                // DEBUG_PRINT("debug: string_fragment unquoted: `%s`\n", ptr->str_frag.value);
                head = split_word(ptr->str_frag.value);
                if (head == NULL) {
                    break;
                }
                string_list *string_list_ptr = head;

                current_string = string_append_back (current_string, string_list_ptr->str);

                while (string_list_ptr->next)
                {
                    string_list_push_back (&string_list_head, &string_list_tail, &current_string);
                    string_list_ptr = string_list_ptr->next;

                    current_string = string_append_back (current_string, string_list_ptr->str);

                    if (! string_list_ptr->next) {
                        break;
                    }
                }
                // check if ptr->str_frag.value have whitespace at the very end
                // so i just finish the string
                if (isspace(string_back(ptr->str_frag.value)))
                {
                    string_list_push_back (&string_list_head, &string_list_tail, &current_string);
                }
                free_string_list (head);
                break;
            default:
                assert (0);
        }
    }
    if (current_string)
    {
        string_list_push_back (&string_list_head, &string_list_tail, &current_string);
    }

    string_list *result = NULL, *result_tail = NULL;

    if (flags & SIM_STR_INT_DO_GLOB)
    {
        // do glob
        for (string_list *ptr = string_list_head; ptr; ptr = ptr->next)
        {
            glob_t glob_results = glob (ptr->str);
            for (size_t i = 0; i < glob_results.gl_pathc; ++i) {
                string tmp = new_string_2 (glob_results.gl_pathv[i]);
                string_list_push_back (&result, &result_tail, &tmp);
            }
            g_globfree (&glob_results);
        }
        // DEBUG_PRINT ("debug: before glob\n");
        // debug_string_list (string_list_head);
        // DEBUG_PRINT ("debug: after glob\n");
        // debug_string_list (result);

        free_string_list (string_list_head);
    }
    else
    {
        result = string_list_head;
        // DEBUG_PRINT ("debug: result (no glob)\n");
        // debug_string_list (result);
    }
    free_string_fragment_list (string_fragment_head);
    // yypush_parse(parser, 0, &yylval);
    // yypstate_delete (parser);
    return result;
}

void interpret_command (yypstate *parser, int token, uint64_t flags, int *saved_token)
{
    int status;

    if (token == TOK_NIL)
    {
        token = token_advance_skip_space_newline (token);
    }

    yypush_parse(parser, PREFIX_COMMAND, NULL);
    // assignments
    while (token == IDENTIFIER_ASSIGNMENT)
    {
        yypush_parse(parser, PREFIX_ASSIGNMENT, NULL);
        string identifier_assignment = yylval.str;
        yypush_parse(parser, token, &yylval);
        string value = yylval.str = string_list_retrieve_string (interpret_string(token_next(), SIM_STR_INT_DO_GLOB, &token));
        yypush_parse(parser, STRING, &yylval);
        token = token_skip_space(token);
    }

    // cont -> token = string | redirection
    for(int cont = 1; cont; token = token_skip_space(token))
    {
        if (token == STRING_LIST)
        {
            string_list *list = interpret_string(STRING_LIST, SIM_STR_INT_DO_GLOB, &token);
            for (string_list *ptr = list; ptr; ptr = ptr->next)
            {
                yylval.str = new_string_2 (ptr->str);
                yypush_parse(parser, STRING, &yylval);
            }
            free_string_list(list);
        }
        else {
            string_list *list;
            switch (token)
            {
                case LESS:
                case GREATER:
                case GREATER_DOUBLE:
                case AND_GREATER:
                case GREATER_AND:
                case AND_GREATER_DOUBLE:
                case LESS_AND:

                    yypush_parse(parser, PREFIX_REDIRECTION, NULL);
                    yypush_parse(parser, token, NULL);
                    token = token_advance_skip_space (token);
                    list = interpret_string (token, SIM_STR_INT_DO_GLOB, &token);
                    assert (list);
                    string file = string_list_retrieve_string (list);
                    if (list->next)
                    {
                        fprintf (stderr, "Yar: `%s`: ambiguous redirection.\n", file);
                        syntax_error = 1;
                    }
                    yylval.str = file;
                    yypush_parse (parser, STRING, &yylval);
                    free_string_list(list);
                    break;
                case NUM_LESS:
                case NUM_GREATER:
                case NUM_LESS_AND:
                case NUM_GREATER_AND:
                    break;
                default:
                    cont = 0;
                    break;
            }
        }
    }

    if (saved_token) {
        *saved_token = token;
    }
}

void interpret_job (int token, uint64_t flags, int *saved_token)
{
    int status;
    yypstate *parser = yypstate_new ();

    if (token == TOK_NIL)
    {
        token = token_advance_skip_space_newline (token);
    }

    yypush_parse(parser, PREFIX_JOB, NULL);

    for (int cont = 1; cont;) 
    {
        switch (token)
        {
            case NEWLINE:
            case SEMICOLON:
            case AMPERSAND:
                yypush_parse (parser, token, NULL);
                cont = 0;
                token = token_advance_skip_space(token);
                break;
            case PIPE:
                yypush_parse (parser, token, NULL);
                token = token_advance_skip_space (token);
                interpret_command (parser, token, 0, &token);
                token = token_skip_space (token);
                break;
            default:
                if (token == IDENTIFIER_ASSIGNMENT || token == STRING_LIST || check_redirection (token)) {
                    interpret_command (parser, token, 0, &token);
                    token = token_skip_space (token);
                }
                else {
                    cont = 0;
                }
                break;
        }
    }

    if (saved_token) {
        *saved_token = token;
    }

    // push eof to make parser finalize
    status = yypush_parse (parser, YYEOF, &yylval);
    yypstate_delete (parser);
}

// parse program_segment
void interpret(const char *source)
{
    runtime_error = 0;
    syntax_error = 0;
    init_lexer();

    YY_BUFFER_STATE buffer = yy_scan_string(source);

    // yypstate *parser = yypstate_new ();
    // yypstate_delete (parser);
    interpret_job (TOK_NIL, 0, NULL);
    if (syntax_error == 0 && runtime_error == 0) {
        launch_job(extracted_job);
    }
    if (extracted_job)
    {
        extracted_job = NULL;
    }

    // yypush_parse(parser, 0, &yylval);
    yy_delete_buffer(buffer);
}

