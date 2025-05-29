#include "yar_interpreter.h"

#include "data_structure/string.h"
#include "yar_ast.h"
#include "yar_debug.h"
#include "yar_parser.tab.h"
#include "yar_lexer.h"
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

void string_list_push_back (string_list **head, string_list **tail, string *str)
{
    string_list *elem = new_string_list (*str);
    if (*head) {
        (*tail)->next = elem;
        (*tail) = elem;
    }
    else {
        (*head) = (*tail) = elem;
    }
    *str = NULL;
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
        if (string_fragment_tail == NULL) {
            string_fragment_head = string_fragment_tail = new_string_fragment_list (str_frag);
        }
        else {
            string_fragment_tail->next = new_string_fragment_list (str_frag);
            string_fragment_tail = string_fragment_tail->next;
        }
    }

    string_list *string_list_head = NULL, *string_list_tail = NULL;
    string last_string = NULL;

    for (string_fragment_list *ptr = string_fragment_head; ptr; ptr = ptr->next)
    {
        // do not free this
        string content = ptr->str_frag.value;

        if (ptr->str_frag.quoted)
        {
            DEBUG_PRINT("debug: string_fragment quoted: `%s`\n", content);
            if (last_string)
            {
                last_string = string_append_back(last_string, content);
            }
            else {
                last_string = new_string_2(content);
            }
        }
        else {
            DEBUG_PRINT("debug: start un-quoted string: `%s`\n", ptr->str_frag.value);
            string_list *head = split_word(ptr->str_frag.value);
            assert (head != NULL);
            string_list *string_list_ptr = head;
            last_string = last_string ? string_append_back (last_string, string_list_ptr->str) : new_string_2 (string_list_ptr->str);
            DEBUG_PRINT("debug: 1st last_string: `%s`\n", last_string);
            while (string_list_ptr->next)
            {
                string_list_push_back (&string_list_head, &string_list_tail, &last_string);
                string_list_ptr = string_list_ptr->next;
                last_string = new_string_2(string_list_ptr->str);
                if (! string_list_ptr->next) {
                    break;
                }
                DEBUG_PRINT("debug: ?th last_string: `%s`\n", last_string);
            }
            // check if string_list_ptr->str_frag.value have whitespace at the very end
            // so i just finish the string
            if (isspace(string_back(content)))
            {
                DEBUG_PRINT("the un-quoted string have space at end, so end it\n");
                string_list_push_back (&string_list_head, &string_list_tail, &last_string);
            }
            free_string_list (head);
        }
    }
    if (last_string)
    {
        DEBUG_PRINT("debug: last last_string: `%s`\n", last_string);
        string_list_push_back (&string_list_head, &string_list_tail, &last_string);
    }

    for (string_list *ptr = string_list_head; ptr; ptr = ptr->next)
    {
        DEBUG_PRINT("debug: string: `%s`\n", ptr->str);
    }

    // free string_fragment_list
    for (string_fragment_list *ptr = string_fragment_head, *nxt; ptr; )
    {
        nxt = ptr->next;
        free_string (ptr->str_frag.value);
        free (ptr);
        ptr = nxt;
    }

    // yypush_parse(parser, 0, &yylval);
    // yypstate_delete (parser);
    return string_list_head;
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
    interpret_string(0);

    // yypush_parse(parser, 0, &yylval);
    yy_delete_buffer(buffer);
}

