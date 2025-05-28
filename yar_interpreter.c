#include "yar_interpreter.h"

#include "data_structure/string.h"
#include "yar_parser.tab.h"
#include "yar_lexer.h"

#ifdef DEBUG
#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG_PRINT(...) ((void)0)
#endif

int token;
YYSTYPE yylval;

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

string interpret_string ()
{
    // yypstate *parser = yypstate_new ();
    int status;

    string result = new_string();

    while (token == STRING)
    {
        result = string_append_back(result, yylval.str);
        free_string(yylval.str);
        token = token_next();
    }

    // yypush_parse(parser, 0, &yylval);
    // yypstate_delete (parser);
    return result;
}

void interpret_command ()
{
    yypstate *parser = yypstate_new ();
    int status;

    yypush_parse(parser, PREFIX_COMMAND, NULL);
    // assignments
    while (token == IDENTIFIER_ASSIGNMENT)
    {
        yypush_parse(parser, PREFIX_ASSIGNMENT, NULL);
        string identifier_assignment = yylval.str;
        yypush_parse(parser, token, &yylval);
        token = token_next();
        string value = yylval.str = interpret_string();
        yypush_parse(parser, STRING, &yylval);
        free_string(identifier_assignment);
        free_string (value);

        token = token_skip_whitespace();
    }

    // cont -> token = string | redirection
    for(int cont = 1; cont; token = token_skip_whitespace())
    {
        if (token == STRING)
        {
            yylval.str = interpret_string();
            yypush_parse(parser, STRING, &yylval);
        }
        else {
            switch (token)
            {
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

                    yypush_parse(parser, PREFIX_REDIRECTION, NULL);
                    yypush_parse(parser, token, NULL);
                    token = token_skip_whitespace();
                    string file = yylval.str = interpret_string();
                    yypush_parse(parser, STRING, &yylval);
                    break;
                case NEWLINE:
                case SEMICOLON:
                    yypush_parse(parser, token, NULL);
                default:
                    cont = 0;
                    break;
            }
        }
    }

    yypush_parse(parser, 0, &yylval);
    yypstate_delete (parser);
}

// parse program_segment
void interpret(char *source)
{
    YY_BUFFER_STATE buffer = yy_scan_string(source);

    // yypstate *parser = yypstate_new ();
    // yypstate_delete (parser);
    token = token_skip_whitespace_newline();
    interpret_command();

    // yypush_parse(parser, 0, &yylval);
    yy_delete_buffer(buffer);
}

