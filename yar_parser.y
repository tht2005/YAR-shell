%define api.push-pull push
%define api.pure full
%define parse.error verbose

%code requires {
#include "yar_ast.h"
#include "yar_exec.h"
#include "yar_job.h"
#include "data_structure/string.h"
#include "yar_interpreter.h"
}

%{

#include <stdio.h>
#include <stdlib.h>
#include "yar_debug.h"
#include "yar_job.h"

int yylex(void);
void yyerror(const char *s);

extern job *extracted_job;

%}

%union {
    string str;
    string_fragment str_frag;
    redirection redirection;
    string_list *str_list;
    argument_list *argument_list;
    command *command;
    process *process;
    job *job;
}

%token TOK_NIL

%token PREFIX_PROGRAM_SEGMENT
%token PREFIX_FUNCTION
%token PREFIX_CODEBLOCK
%token PREFIX_ASSIGNMENT
%token PREFIX_REDIRECTION
%token PREFIX_COMMAND
%token PREFIX_JOB

%token PREFIX_SUBSTITUTION_STRING
%token PREFIX_SUBSTITUTION_COMMAND 
%token PREFIX_SUBSTITUTION_ARITHMETIC 

%token <str> IDENTIFIER_ASSIGNMENT
%token <str> STRING
%token STRING_LIST
%token SEMICOLON SEMICOLON_DOUBLE
%token NEWLINE

%token PIPE
%token AMPERSAND
%token CODEBLOCK_BEGIN          // {{
%token CODEBLOCK_END            // }}

%token LESS GREATER GREATER_DOUBLE AND_GREATER GREATER_AND AND_GREATER_DOUBLE LESS_AND
%token NUM_LESS NUM_GREATER NUM_LESS_AND NUM_GREATER_AND
%token PLUS MINUS TIMES DIVIDE

%token BRACE_LEFT BRACE_RIGHT

%token DOUBLE_QUOTE

// token for lexer, do not use in grammar
%token WHITESPACE

%right PIPE
%right AMPERSAND

%type <str> assignment
%type <redirection> redirection
%type <str_list> assignment_list
%type <argument_list> arguments_and_redirections_list
%type <process> command
%type <job> job_background job_pipeline job_command

%start input

%%

input:          PREFIX_JOB job_pipeline 
                                {
                                    extracted_job = $2;
                                }
     |          PREFIX_JOB job_background
                                {
                                    extracted_job = $2;
                                }
     ;

splitter:       SEMICOLON
        |       NEWLINE
        ;
 
program_segment:    PREFIX_PROGRAM_SEGMENT
               |    program_segment function
               |    codeblock
               ;

codeblock:      PREFIX_CODEBLOCK CODEBLOCK_BEGIN CODEBLOCK_END
         ;
function:;

statement:;

job_background:     job_pipeline AMPERSAND
                                {
                                    $$ = $1;
                                    $$->foreground = 0;
                                }
              ;

job_pipeline:       job_command
                                {
                                    $$ = $1;
                                }
            |       job_command PIPE job_pipeline
                                {
                                    process *p = $1->first_process;
                                    $1->first_process = NULL;
                                    free_job ($1);
                                    p->next = $3->first_process;
                                    $3->first_process = p;
                                    $$ = $3;
                                }
            ;

job_command:        command
                                {
                                    job *job = new_job ();
                                    job->first_process = $1;
                                    $$ = job;
                                }
           |        codeblock
           ;

command:        PREFIX_COMMAND assignment_list arguments_and_redirections_list
                                                                    {
                                                                        command *command = new_command ($2, $3);
                                                                        $$ = __ast_build_process (command);
                                                                        free_command (command);
                                                                    }
       ;

assignment_list:    /* empty */
                                                                    {
                                                                        $$ = NULL;
                                                                    }
               |    assignment assignment_list                      {
                                                                        string_list *list = new_string_list ($1);
                                                                        list->next = $2;
                                                                        $$ = list;
                                                                    }
               ;
assignment:     PREFIX_ASSIGNMENT IDENTIFIER_ASSIGNMENT STRING      {
                                                                        $$ = new_string_2 ($2);
                                                                        $$ = string_append_back ($$, $3);
                                                                    }
          ;

arguments_and_redirections_list:    /* empty */
                                                                                        {
                                                                                            $$ = NULL;
                                                                                        }
                               |    STRING arguments_and_redirections_list 
                                                                                        {
                                                                                            argument_list *list = new_argument_list ();
                                                                                            list->type = AL_ARGUMENT;
                                                                                            list->arg = $1;
                                                                                            list->next = $2;
                                                                                            $$ = list;
                                                                                            DEBUG_PRINT("argument_list: argument `%s` append to list\n", $1);
                                                                                        }
                               |    redirection arguments_and_redirections_list 
                                                                                        {
                                                                                            DEBUG_PRINT("redirection_list: redirection (%d) `%s` append to list\n", $1.type, $1.file);
                                                                                            argument_list *list = new_argument_list();
                                                                                            list->type = AL_REDIRECTION;
                                                                                            list->redirection = $1;
                                                                                            list->next = $2;
                                                                                            $$ = list;
                                                                                        }
                               ;

redirection:    PREFIX_REDIRECTION LESS STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", LESS, $3);
                                                                        $$ = make_redirection (LESS, $3);
                                                                    }
           |    PREFIX_REDIRECTION NUM_LESS STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", NUM_LESS, $3);
                                                                        $$ = make_redirection (NUM_LESS, $3);
                                                                    }
           |    PREFIX_REDIRECTION GREATER STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", GREATER, $3);
                                                                        $$ = make_redirection (GREATER, $3);
                                                                    }
           |    PREFIX_REDIRECTION NUM_GREATER STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", NUM_GREATER, $3);
                                                                        $$ = make_redirection (NUM_GREATER, $3);
                                                                    }
           |    PREFIX_REDIRECTION GREATER_DOUBLE STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", GREATER_DOUBLE, $3);
                                                                        $$ = make_redirection (GREATER_DOUBLE, $3);
                                                                    }
           |    PREFIX_REDIRECTION AND_GREATER STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", AND_GREATER, $3);
                                                                        $$ = make_redirection (AND_GREATER, $3);
                                                                    }
           |    PREFIX_REDIRECTION GREATER_AND STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", GREATER_AND, $3);
                                                                        $$ = make_redirection (GREATER_AND, $3);
                                                                    }
           |    PREFIX_REDIRECTION AND_GREATER_DOUBLE STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", AND_GREATER_DOUBLE, $3);
                                                                        $$ = make_redirection (AND_GREATER_DOUBLE, $3);
                                                                    }
           |    PREFIX_REDIRECTION LESS_AND STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", LESS_AND, $3);
                                                                        $$ = make_redirection (LESS_AND, $3);
                                                                    }
           |    PREFIX_REDIRECTION NUM_LESS_AND STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", NUM_LESS_AND, $3);
                                                                        $$ = make_redirection (NUM_LESS_AND, $3);
                                                                    }
           |    PREFIX_REDIRECTION NUM_GREATER_AND STRING
                                                                    {
                                                                        DEBUG_PRINT("debug: redirection %d `%s`\n", NUM_GREATER_AND, $3);
                                                                        $$ = make_redirection (NUM_GREATER_AND, $3);
                                                                    }
           /* and more ... */
           ;

substitution:       substitution_command
            |       substitution_arithmetic
            ;

substitution_command:   PREFIX_SUBSTITUTION_COMMAND "$(" program_segment ")"
                    ;

substitution_arithmetic:    PREFIX_SUBSTITUTION_ARITHMETIC "$((" "))"

%%

void yyerror(const char *s) {
    fprintf(stderr, "Yar: %s\n", s);
    syntax_error = 1;
}

