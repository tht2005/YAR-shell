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

string __add_escape_character (string input);

int in_double_quote;
void init_lexer ()
{
    in_double_quote = 0;
}

%}

%option             noyywrap
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
%s                  BRACE_STRING

/*
#.*$                        {
                            }

{WHITESPACE}+               { 
                            }

<<EOF>>                     { 
                                return ENDOFFILE; 
                            }

"="                         { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('=');
                                    return token;
                                }
                                return EQUAL; 
                            }

"<"                         { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('<');
                                    return token;
                                }
                                return LESS; 
                            }

">"                         { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('>');
                                    return token;
                                }
                                return GREATER; 
                            }

">>"                        { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('>'); unput('>');
                                    return token;
                                }
                                return GREATER_DOUBLE; 
                            }

"<&"                        { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('&'); unput('<');
                                    return token;
                                }
                                return LESS_AND; 
                            }

">&"                        { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('&'); unput('>');
                                    return token;
                                }
                                return GREATER_AND; 
                            }

"&>"                        { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('>'); unput('&');
                                    return token;
                                }
                                return AND_GREATER; 
                            }

"&>>"                       { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('>'); unput('>'); unput('&');
                                    return token;
                                }
                                return AND_GREATER_DOUBLE; 
                            }

"<>"                        { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('>'); unput('<');
                                    return token;
                                }
                                return LESS_GREATER; 
                            }

"!"                         { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('!');
                                    return token;
                                }
                                return BANG; 
                            }

"|"                         { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('|');
                                    return token;
                                }
                                return OR; 
                            }

"&"                         { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('&');
                                    return token;
                                }
                                return AND; 
                            }

"||"                        { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('|'); unput('|');
                                    return token;
                                }
                                return OR_DOUBLE; 
                            }

"&&"                        { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('&'); unput('&');
                                    return token;
                                }
                                return AND_DOUBLE; 
                            }

"+"                         { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('+');
                                    return token;
                                }
                                return PLUS; 
                            }

"-"                         { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('-');
                                    return token;
                                }
                                return MINUS; 
                            }

"*"                         { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('*');
                                    return token;
                                }
                                return TIMES; 
                            }

"/"                         { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('/');
                                    return token;
                                }
                                return DIVIDE; 
                            }

"(("                        { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('('); unput('(');
                                    return token;
                                }
                                return ROUND_LPAREN_DOUBLE; 
                            }

"))"                        { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput(')'); unput(')');
                                    return token;
                                }
                                return ROUND_RPAREN_DOUBLE; 
                            }

"[["                        { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('['); unput('[');
                                    return token;
                                }
                                return SQUARE_LPAREN_DOUBLE; 
                            }

"]]"                        { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput(']'); unput(']');
                                    return token;
                                }
                                return SQUARE_RPAREN_DOUBLE; 
                            }

"("                         { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('(');
                                    return token;
                                }
                                return ROUND_LPAREN; 
                            }

")"                         { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput(')');
                                    return token;
                                }
                                return ROUND_RPAREN; 
                            }

"["                         { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('[');
                                    return token;
                                }
                                return SQUARE_LPAREN; 
                            }

"]"                         { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput(']');
                                    return token;
                                }
                                return SQUARE_RPAREN; 
                            }

"{"                         { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('{');
                                    return token;
                                }
                                return CURLY_LPAREN; 
                            }

"}"                         { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput('}');
                                    return token;
                                }
                                return CURLY_RPAREN; 
                            }

";;"                        { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput(';'); unput(';');
                                    return token;
                                }
                                return SEMICOLON_DOUBLE; 
                            }

";"                         { 
                                if (in_word) {
                                    int token = finish_word();
                                    unput(';');
                                    return token;
                                }
                                return SEMICOLON; 
                            }

"if"                        { 
                                if (in_word) {
                                    add_to_word(yytext);
                                } else {
                                    return IF;
                                }
                            }

"then"                      { 
                                if (in_word) {
                                    add_to_word(yytext);
                                } else {
                                    return THEN;
                                }
                            }

"elif"                      { 
                                if (in_word) {
                                    add_to_word(yytext);
                                } else {
                                    return ELIF;
                                }
                            }

"else"                      { 
                                if (in_word) {
                                    add_to_word(yytext);
                                } else {
                                    return ELSE;
                                }
                            }

"fi"                        { 
                                if (in_word) {
                                    add_to_word(yytext);
                                } else {
                                    return FI;
                                }
                            }

"for"                       { 
                                if (in_word) {
                                    add_to_word(yytext);
                                } else {
                                    return FOR;
                                }
                            }

"in"                        { 
                                if (in_word) {
                                    add_to_word(yytext);
                                } else {
                                    return IN;
                                }
                            }

"do"                        { 
                                if (in_word) {
                                    add_to_word(yytext);
                                } else {
                                    return DO;
                                }
                            }

"done"                      { 
                                if (in_word) {
                                    add_to_word(yytext);
                                } else {
                                    return DONE;
                                }
                            }

"while"                     { 
                                return WHILE;
                            }

"until"                     { 
                                return UNTIL;
                            }

"case"                      { 
                                return CASE;
                            }

"esac"                      { 
                                return ESAC;
                            }

"break"                     { 
                                return BREAK;
                            }

"continue"                  { 
                                return CONTINUE;
                            }

"return"                    { 
                                return RETURN;
                            }

"exit"                      { 
                                return EXIT;
                            }

"exec"                      { 
                                return EXEC;
                            }

"trap"                      { 
                                return TRAP;
                            }

"time"                      { 
                                return TIME;
                            }

"coproc"                    { 
                            }
*/

%%

\${IDENTIFIER}          {
                            DEBUG_PRINT("debug: flex catch variable substitution: `%s`\n", yytext);
                            yylval->str_frag = make_string_fragment(STR_FRAG_NON_QUOTED,
                                                    variable_substitution_handler(yytext));
                            return STRING;
                        }

\$\{([^\\}]|\\.)*\}     {
                            DEBUG_PRINT("debug: flex catch string substitution: `%s`\n", yytext);
                            yylval->str_frag = make_string_fragment(STR_FRAG_NON_QUOTED,
                                                    string_substitution_handler(yytext));
                            return STRING;
                        }

<DOUBLE_QUOTE_STRING>{
    {WHITESPACE}+       {
                            string content = new_string_2 (yytext);
                            yylval->str_frag = make_string_fragment (STR_FRAG_QUOTED,
                                                                    content);
                            return STRING;
                        }
    ([^ \t\r\n\"]|\\.)+ {
                            yylval->str_frag = make_string_fragment (STR_FRAG_QUOTED,
                                                                    new_string_2 (yytext));
                            return STRING;
                        }
    \n                  {
                            ++yylineno;
                            yylval->str_frag = make_string_fragment (STR_FRAG_QUOTED,
                                                                    new_string_2 (yytext));
                            return STRING;
                        }
    \"                  {
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
                            return STRING;
                        }
}
\"                      {
                            yy_push_state (DOUBLE_QUOTE_STRING);
                            string content = simple_string_interpret (SIM_STR_INT_DOUBLE_QUOTE);
                            content = __add_escape_character (content);
                            yylval->str_frag = make_string_fragment (STR_FRAG_QUOTED,
                                                                    content);
                            return STRING;
                        }

{WHITESPACE}+ {
    return WHITESPACE;
}
{IDENTIFIER}"=" {
    yylval->str = new_string_2(yytext);
    return IDENTIFIER_ASSIGNMENT;
}

\{                      {
                            // call yylex get next token ...
                        }

\\.                     {
                            DEBUG_ASSERT (strlen(yytext) == 2);
                            string content = new_string_2 (yytext + 1);
                            yylval->str_frag = make_string_fragment (STR_FRAG_NON_QUOTED, content);
                            return STRING;
                        }

\'([^\'])*\'            {
                            string content = new_substr (yytext + 1, yytext + strlen(yytext) - 1);
                            content = __add_escape_character (content);
                            yylval->str_frag = make_string_fragment(STR_FRAG_QUOTED, content);
                            return STRING;
                        }
([^ \=\t\r\n\"\'()<>&|$;]|\\[ ])+ {
                                    yylval->str_frag = make_string_fragment(STR_FRAG_NON_QUOTED,
                                                            new_string_2(yytext) );
                                    return STRING;
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
"<&"        {
    return LESS_AND;
}
{DIGIT}+"<&"    {
    return NUM_LESS_AND;
}
{DIGIT}+">&"    {
    return NUM_GREATER_AND;
}

.                       {
                            fprintf (stderr, "Unexpected character: %c\n", yytext[0]);
                        }
<<EOF>>                 {
                            return 0;
                        }

%%

string __add_escape_character (string input)
{
    string res = new_string ();
    size_t len = string_length (input);
    for (size_t i = 0; i < len; ++i)
    {
        res = string_push_back (res, '\\');
        if (input[i] == '\\')
        {
            ++i;
        }
        if (i < len) {
            res = string_push_back (res, input[i]);
        }
    }
    free_string (input);
    return res;
}

