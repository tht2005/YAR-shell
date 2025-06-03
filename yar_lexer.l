%{
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_structure/string.h"
#include "yar_ast.h"
#include "yar_parser.tab.h"
#include "yar_debug.h"
#include "yar_substitution.h"
#include "yar_interpreter.h"

string __add_escape_character_all (string str);
string __add_escape_character_except (string str, const char *delim);
string __add_escape_character_except_space (string str);

string single_quote_str_buf ;

int in_double_quote;
int in_single_quote;
int arith_mode;
int comment_mode;
// int treat_brace_as_string;
void init_lexer ()
{
    in_double_quote = 0;
    in_single_quote = 0;
    arith_mode = 0;
    comment_mode = 0;
}

int not_split_expansion ()
{
    return in_double_quote;
}
int keep_whitespace_as_string ()
{
    return in_double_quote;
}
int store_identifier_as_string ()
{
    return in_double_quote;
}
int quote_expansion ()
{
    return in_double_quote;
}
int treat_num_sign_as_string ()
{
    return in_double_quote;
}

// int yypeeklex ();

%}

%option             noyywrap
%option             never-interactive
%option             yylineno
%option             stack
%option             bison-bridge 
%option             header-file="yar_lexer.h"
%option             outfile="yar_lexer.c"

DIGIT               [0-9]
LETTER              [a-zA-Z_]
ALNUM               [a-zA-Z0-9_]
WHITESPACE          [ \t]
IDENTIFIER          {LETTER}{ALNUM}*

%s                  DOUBLE_QUOTE_STRING
%x                  SINGLE_QUOTE_STRING
%x                  COMMENT

%%

\${IDENTIFIER}          {
                            DEBUG_PRINT("debug: flex catch variable substitution: `%s`\n", yytext);
                            string content = variable_substitution_handler(yytext);
                            if (not_split_expansion ())
                            {
                                content = __add_escape_character_all (content);
                                yylval->str_frag = make_string_fragment (STR_FRAG_QUOTED, content);
                            }
                            else {
                                content = __add_escape_character_except_space (content);
                                yylval->str_frag = make_string_fragment (STR_FRAG_NON_QUOTED, content);
                            }
                            return STRING_LIST;
                        }

\$\{([^\\}]|\\.)*\}     {
                            DEBUG_PRINT("debug: flex catch string substitution: `%s`\n", yytext);
                            string content = string_substitution_handler(yytext);
                            yylval->str_frag = make_string_fragment (STR_FRAG_NON_QUOTED, content);
                            if (not_split_expansion ())
                            {
                                content = __add_escape_character_all (content);
                                yylval->str_frag = make_string_fragment (STR_FRAG_QUOTED, content);
                            }
                            else {
                                content = __add_escape_character_except_space (content);
                                yylval->str_frag = make_string_fragment (STR_FRAG_NON_QUOTED, content);
                            }
                            return STRING_LIST;
                        }

<DOUBLE_QUOTE_STRING>{
    ([^ \t\r\n\"]|\\.|\\[ ])+   {
                            yylval->str_frag = make_string_fragment (STR_FRAG_QUOTED,
                                                                    new_string_2 (yytext));
                            return STRING_LIST;
                        }
    \n                  {
                            ++yylineno;
                            yylval->str_frag = make_string_fragment (STR_FRAG_QUOTED,
                                                                    new_string_2 (yytext));
                            return STRING_LIST;
                        }
    \"                  {
                            in_double_quote = 0;
                            yy_pop_state ();
                            return DOUBLE_QUOTE;
                        }
    .                   {
                            DEBUG_ASSERT (strlen(yytext) == 1);
                            if (yytext[0] == '\n')
                            {
                                ++yylineno;
                            }
                            string content = new_string_2 (yytext);
                            yylval->str_frag = make_string_fragment(STR_FRAG_QUOTED, content);
                            return STRING_LIST;
                        }
}
\"                      {
                            DEBUG_PRINT ("debug: enter double quoted mode\n");
                            in_double_quote = 1;
                            yy_push_state (DOUBLE_QUOTE_STRING);
                            string_list *list = interpret_string (TOK_NIL, SIM_STR_INT_DOUBLE_QUOTE, NULL);
                            string content;
                            if (list == NULL) {
                                content = new_string(); 
                            }
                            else {
                                DEBUG_ASSERT (list->next == NULL);
                                content = new_string_2 (list->str);
                                content = __add_escape_character_all (content);
                            }
                            DEBUG_PRINT ("debug: `%s`\n", content);
                            yylval->str_frag = make_string_fragment (STR_FRAG_QUOTED, content);
                            free_string_list (list);
                            return STRING_LIST;
                        }

<SINGLE_QUOTE_STRING>{
    \'                  {
                            yy_pop_state ();
                            single_quote_str_buf = __add_escape_character_all (single_quote_str_buf);
                            yylval->str_frag = make_string_fragment (STR_FRAG_QUOTED, single_quote_str_buf);
                            single_quote_str_buf = NULL;
                            return STRING_LIST;
                        }
    [^\']+              {
                            single_quote_str_buf = string_append_back (single_quote_str_buf, yytext);
                        }
}
\'                      {
                            in_single_quote = 1;
                            yy_push_state (SINGLE_QUOTE_STRING);
                            single_quote_str_buf = new_string ();
                        }

{WHITESPACE}+ {
    if (keep_whitespace_as_string ())
    {
        string content = new_string_2 (yytext);
        yylval->str_frag = make_string_fragment (STR_FRAG_QUOTED,
                                                content);
        return STRING_LIST;
    }
    return WHITESPACE;
}

{IDENTIFIER}"=" {
    if (store_identifier_as_string ())
    {
        string content = new_string_2 (yytext);
        yylval->str_frag = make_string_fragment (STR_FRAG_QUOTED,
                                                content);
        return STRING_LIST;
    }
    yylval->str = new_string_2(yytext);
    return IDENTIFIER_ASSIGNMENT;
}

<COMMENT>{
    \n                  {
                            // skip all the   # ..... \n
                            // return newline token as the comment should not be exist
                            comment_mode = 0;
                            yy_pop_state ();
                            return NEWLINE;
                        }
    [^\n]+              ;
}
\#                      {
                            if (treat_num_sign_as_string ())
                            {
                                string content = new_string_2 (yytext);
                                yylval->str_frag = make_string_fragment (STR_FRAG_QUOTED, content);
                                return STRING_LIST;
                            }
                            else {
                                // enter comment state
                                comment_mode = 1;
                                yy_push_state (COMMENT);
                            }
                        }

\\.                     {
                            DEBUG_ASSERT (strlen(yytext) == 2);
                            string content = new_string_2 (yytext + 1);
                            yylval->str_frag = make_string_fragment (STR_FRAG_QUOTED, content);
                            return STRING_LIST;
                        }

\n          {
    ++yylineno;
    return NEWLINE;
}
;           {
    return SEMICOLON;
}
;;          {
    return SEMICOLON_DOUBLE;
}

"{{"        {
    return CODEBLOCK_BEGIN;
}
"}}"        {
    return CODEBLOCK_END;
}
"|"         {
    return PIPE;
}
"&"         {
    return AMPERSAND;
}
"<"         {
    return LESS;
}
{DIGIT}+"<" {
    return NUM_LESS;
}
">"         {
    return GREATER;
}
{DIGIT}+">" {
    return NUM_GREATER;
}
">>"        {
    return GREATER_DOUBLE;
}
"&>"        {
    return AND_GREATER;
}
">&"        {
    return GREATER_AND;
}
"&>>"       {
    return AND_GREATER_DOUBLE;
}
{DIGIT}+"<&"    {
    return NUM_LESS_AND;
}
{DIGIT}+">&"    {
    return NUM_GREATER_AND;
}

([^ \=\#\t\r\n\"\'()<>&\|$;{}]|\\.|\\[ ]|[{}])+     {
                                    yylval->str_frag = make_string_fragment(STR_FRAG_QUOTED,
                                                            new_string_2(yytext) );
                                    return STRING_LIST;
                                }

.                       {
                            fprintf (stderr, "Unexpected character: %c\n", yytext[0]);
                        }
<<EOF>>                 {
                            // DEBUG_PRINT ("flex: debug: exit with state (%d)\n", YY_START);
                            if (comment_mode)
                            {
                                // skip all the   # ..... <<EOF>>
                                // return newline token as the comment should not be exist
                                // call for new input ...
                                comment_mode = 0;
                                yy_pop_state ();
                                return NEWLINE;
                            }
                            else if (in_double_quote)
                            {

                            }
                            else if (in_single_quote)
                            {

                            }
                            else if (arith_mode)
                            {

                            }
                            return 0;
                        }

%%

string __add_escape_character_all (string str)
{
    string res = new_string ();
    size_t len = string_length (str);
    for (size_t i = 0; i < len; ++i)
    {
        res = string_push_back (res, '\\');
        if (str[i] == '\\')
        {
            ++i;
        }
        if (i < len) {
            res = string_push_back (res, str[i]);
        }
    }
    free_string (str);
    return res;
}
string __add_escape_character_except (string str, const char *delim)
{
    string res = new_string ();
    size_t len = string_length (str);
    for (size_t i = 0; i < len; ++i)
    {
        if (strchr (delim, str[i])) {
            res = string_push_back (res, str[i]);
            continue;
        }
        res = string_push_back (res, '\\');
        if (str[i] == '\\')
        {
            ++i;
        }
        if (i < len) {
            res = string_push_back (res, str[i]);
        }
    }
    free_string (str);
    return res;
}

string __add_escape_character_except_space (string str)
{
    return __add_escape_character_except (str, " \t\r");
}

// void free_metadata (int token, YYSTYPE *val)
// {
//     switch (token)
//     {
//         case STRING:
//             free (val->str_frag.value);
//             break;
//         case IDENTIFIER_ASSIGNMENT:
//             free_string (val->str);
//             break;
//         default:
//             break;
//     }
// }
//
// int yypeeklex ()
// {
//     YYSTYPE tmp;
//     int token = yylex (&tmp);
//     free_metadata (token, &tmp);
//     for (size_t i = strlen (yytext); i-- > 0; )
//         unput (yytext[i]);
//     return token;
// }
