#include "yar_interpreter.h"

#include "data_structure/string.h"
#include "yar_ast.h"
#include "yar_debug.h"
#include "yar_parser.tab.h"
#include "yar_lexer.h"

#include "libglob/glob.h"

#include <string.h>
#include <ctype.h>

#define WHITESPACE_CHARACTERS       " \t\r"

int token;
YYSTYPE yylval;

int check_argument_or_redirection ()
{
    switch (token)
    {
        case STRING:

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

int token_next ()
{
    return yylex (&yylval);
}

int token_skip_whitespace ()
{
    while ((token = token_next()) == WHITESPACE)
    {
        // do nothing
    }
    return token;
}
int token_skip_whitespace_newline ()
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
    g_glob (pattern, GLOB_BRACE | GLOB_MARK | GLOB_NOCHECK | GLOB_TILDE, NULL, &results);
    return results;
}

string_list *split_word (const char *input)
{
    char *copy = strdup (input);
    char *saveptr;

    string_list *string_list_head = NULL, *string_list_tail = NULL;
    char *token = strtok_r (copy, WHITESPACE_CHARACTERS, &saveptr);
    while (token != NULL)
    {
        string new_str = new_string_2(token);
        string_list_push_back (&string_list_head, &string_list_tail, &new_str);
        token = strtok_r (NULL, WHITESPACE_CHARACTERS, &saveptr);
    }

    free (copy);
    return string_list_head;
}

string_list *interpret_string (int flags)
{
    // yypstate *parser = yypstate_new ();
    // int status;

    string_fragment_list *string_fragment_head = NULL, *string_fragment_tail = NULL;
    for (; token == STRING; token = token_next())
    {
        string_fragment str_frag = yylval.str_frag;
        string_fragment_list_push_back (&string_fragment_head, &string_fragment_tail, &str_frag);
    }

    string_list *string_list_head = NULL, *string_list_tail = NULL;
    string current_string = NULL;

    for (string_fragment_list *ptr = string_fragment_head; ptr; ptr = ptr->next)
    {
        switch (ptr->str_frag.type)
        {
            case STR_FRAG_QUOTED:
                DEBUG_PRINT("debug: string_fragment quoted: `%s`\n", ptr->str_frag.value);
                current_string = string_append_back (current_string, ptr->str_frag.value);
                break;
            case STR_FRAG_NON_QUOTED:
                DEBUG_PRINT("debug: string_fragment unquoted: `%s`\n", ptr->str_frag.value);
                string_list *head = split_word(ptr->str_frag.value);
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

    // do glob
    string_list *result = NULL, *result_tail = NULL;
    for (string_list *ptr = string_list_head; ptr; ptr = ptr->next)
    {
        glob_t glob_results = glob (ptr->str);
        for (size_t i = 0; i < glob_results.gl_pathc; ++i) {
            string tmp = new_string_2 (glob_results.gl_pathv[i]);
            string_list_push_back (&result, &result_tail, &tmp);
        }
        g_globfree (&glob_results);
    }

    DEBUG_PRINT ("before glob\n");
    debug_string_list (string_list_head);
    DEBUG_PRINT ("after glob\n");
    debug_string_list (result);
    free_string_fragment_list (string_fragment_head);
    free_string_list (string_list_head);
    // yypush_parse(parser, 0, &yylval);
    // yypstate_delete (parser);
    return result;
}

void interpret_command ()
{
    // yypstate *parser = yypstate_new ();
    // int status;
    //
    // yypush_parse(parser, PREFIX_COMMAND, NULL);
    // // assignments
    // while (token == IDENTIFIER_ASSIGNMENT)
    // {
    //     yypush_parse(parser, PREFIX_ASSIGNMENT, NULL);
    //     string identifier_assignment = yylval.str;
    //     yypush_parse(parser, token, &yylval);
    //     token = token_next();
    //     string value = yylval.str = interpret_string();
    //     yypush_parse(parser, STRING, &yylval);
    //     free_string(identifier_assignment);
    //     free_string (value);
    //
    //     token = token_skip_whitespace();
    // }
    //
    // // cont -> token = string | redirection
    // for(int cont = 1; cont; token = token_skip_whitespace())
    // {
    //     if (token == STRING)
    //     {
    //         yylval.str = interpret_string();
    //         yypush_parse(parser, STRING, &yylval);
    //     }
    //     else {
    //         switch (token)
    //         {
    //             case LESS:
    //             case GREATER:
    //             case GREATER_DOUBLE:
    //             case AND_GREATER:
    //             case GREATER_AND:
    //             case AND_GREATER_DOUBLE:
    //             case LESS_AND:
    //
    //             case NUM_LESS:
    //             case NUM_GREATER:
    //             case NUM_LESS_AND:
    //             case NUM_GREATER_AND:
    //
    //                 yypush_parse(parser, PREFIX_REDIRECTION, NULL);
    //                 yypush_parse(parser, token, NULL);
    //                 token = token_skip_whitespace();
    //                 string file = yylval.str = interpret_string();
    //                 yypush_parse(parser, STRING, &yylval);
    //                 break;
    //             case NEWLINE:
    //             case SEMICOLON:
    //                 yypush_parse(parser, token, NULL);
    //             default:
    //                 cont = 0;
    //                 break;
    //         }
    //     }
    // }
    //
    // yypush_parse(parser, 0, &yylval);
    // yypstate_delete (parser);
}

// parse program_segment
void interpret(char *source)
{
    YY_BUFFER_STATE buffer = yy_scan_string(source);

    // yypstate *parser = yypstate_new ();
    // yypstate_delete (parser);
    token = token_skip_whitespace_newline();
    // interpret_command();
    free_string_list (interpret_string(0));

    // yypush_parse(parser, 0, &yylval);
    yy_delete_buffer(buffer);
}

