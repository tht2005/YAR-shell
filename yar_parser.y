%define api.push-pull push
%define api.pure full
%define parse.error verbose

%code requires {
    #include "yar_ast.h"
}

%{

#include <stdio.h>
#include <stdlib.h>
#include "yar_debug.h"

int yylex(void);
void yyerror(const char *s);
%}

%code requires {
    #include "data_structure/string.h"
}
%union {
    string str;
    string_fragment str_frag;
    redirection redirection;
}

%token PREFIX_PROGRAM_SEGMENT
%token PREFIX_FUNCTION
%token PREFIX_CODEBLOCK
%token PREFIX_ASSIGNMENT
%token PREFIX_REDIRECTION
%token PREFIX_COMMAND

%token PREFIX_SUBSTITUTION_STRING
%token PREFIX_SUBSTITUTION_COMMAND 
%token PREFIX_SUBSTITUTION_ARITHMETIC 

%token <str> IDENTIFIER_ASSIGNMENT
%token <str_frag> STRING
%token SEMICOLON SEMICOLON_DOUBLE
%token NEWLINE

%token LESS GREATER GREATER_DOUBLE AND_GREATER GREATER_AND AND_GREATER_DOUBLE LESS_AND
%token NUM_LESS NUM_GREATER NUM_LESS_AND NUM_GREATER_AND
%token PLUS MINUS TIMES DIVIDE

// token for lexer, do not use in grammar
%token WHITESPACE


%type <str> assignment
%type <redirection> redirection


%%

input:          program_segment
     |          function
     |          command
     |          substitution
     ;

splitter:       SEMICOLON
        |       NEWLINE
        ;

program_segment:    PREFIX_PROGRAM_SEGMENT
               |    program_segment function
               |    codeblock
               ;

codeblock:      PREFIX_CODEBLOCK
         |      statement
         ;

function:;

statement:;

command:        PREFIX_COMMAND assignment_list arguments_and_redirections_list
       ;

assignment_list:    /* empty */
               |    assignment_list assignment                      {
                                                                        // construct assignment list here
                                                                    }
               ;
assignment:     PREFIX_ASSIGNMENT IDENTIFIER_ASSIGNMENT STRING      {
                                                                        $$ = new_string_2 ($2);
                                                                        $$ = string_append_back ($$, $3.value);
                                                                        DEBUG_PRINT("debug: assignment: `%s`\n", $$);
                                                                    }
          ;

arguments_and_redirections_list:    /* empty */
                               |    arguments_and_redirections_list STRING
                                                                                        {
                                                                                            DEBUG_PRINT("debug: argument `%s` append to list\n", $2.value);
                                                                                        }
                               |    arguments_and_redirections_list redirection
                                                                                        {
                                                                                            DEBUG_PRINT("debug: redirection append to list\n");
                                                                                        }
                               ;

redirection:    PREFIX_REDIRECTION LESS STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", LESS, $3.value);
                                                                        $$ = make_redirection (LESS, $3.value);
                                                                    }
           |    PREFIX_REDIRECTION NUM_LESS STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", NUM_LESS, $3.value);
                                                                        $$ = make_redirection (NUM_LESS, $3.value);
                                                                    }
           |    PREFIX_REDIRECTION GREATER STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", GREATER, $3.value);
                                                                        $$ = make_redirection (GREATER, $3.value);
                                                                    }
           |    PREFIX_REDIRECTION NUM_GREATER STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", NUM_GREATER, $3.value);
                                                                        $$ = make_redirection (NUM_GREATER, $3.value);
                                                                    }
           |    PREFIX_REDIRECTION GREATER_DOUBLE STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", GREATER_DOUBLE, $3.value);
                                                                        $$ = make_redirection (GREATER_DOUBLE, $3.value);
                                                                    }
           |    PREFIX_REDIRECTION AND_GREATER STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", AND_GREATER, $3.value);
                                                                        $$ = make_redirection (AND_GREATER, $3.value);
                                                                    }
           |    PREFIX_REDIRECTION GREATER_AND STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", GREATER_AND, $3.value);
                                                                        $$ = make_redirection (GREATER_AND, $3.value);
                                                                    }
           |    PREFIX_REDIRECTION AND_GREATER_DOUBLE STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", AND_GREATER_DOUBLE, $3.value);
                                                                        $$ = make_redirection (AND_GREATER_DOUBLE, $3.value);
                                                                    }
           |    PREFIX_REDIRECTION LESS_AND STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", LESS_AND, $3.value);
                                                                        $$ = make_redirection (LESS_AND, $3.value);
                                                                    }
           |    PREFIX_REDIRECTION NUM_LESS_AND STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", NUM_LESS_AND, $3.value);
                                                                        $$ = make_redirection (NUM_LESS_AND, $3.value);
                                                                    }
           |    PREFIX_REDIRECTION NUM_GREATER_AND STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", NUM_GREATER_AND, $3.value);
                                                                        $$ = make_redirection (NUM_GREATER_AND, $3.value);
                                                                    }
           /* and more ... */
           ;

substitution:       substitution_command
            |       substitution_arithmetic
            |       substitution_string
            ;

substitution_command:   PREFIX_SUBSTITUTION_COMMAND "$(" program_segment ")"
                    ;

substitution_arithmetic:    PREFIX_SUBSTITUTION_ARITHMETIC "$((" "))"

substitution_string:    PREFIX_SUBSTITUTION_STRING "${" STRING "}"
                   ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Yar: Syntax error: %s\n", s);
}

