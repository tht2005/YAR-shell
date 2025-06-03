/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_YAR_PARSER_TAB_H_INCLUDED
# define YY_YY_YAR_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 5 "yar_parser.y"

#include "yar_ast.h"
#include "yar_exec.h"
#include "yar_job.h"
#include "data_structure/string.h"
#include "yar_interpreter.h"

#line 57 "yar_parser.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    TOK_NIL = 258,                 /* TOK_NIL  */
    PREFIX_PROGRAM_SEGMENT = 259,  /* PREFIX_PROGRAM_SEGMENT  */
    PREFIX_FUNCTION = 260,         /* PREFIX_FUNCTION  */
    PREFIX_CODEBLOCK = 261,        /* PREFIX_CODEBLOCK  */
    PREFIX_ASSIGNMENT = 262,       /* PREFIX_ASSIGNMENT  */
    PREFIX_REDIRECTION = 263,      /* PREFIX_REDIRECTION  */
    PREFIX_COMMAND = 264,          /* PREFIX_COMMAND  */
    PREFIX_JOB = 265,              /* PREFIX_JOB  */
    PREFIX_SUBSTITUTION_STRING = 266, /* PREFIX_SUBSTITUTION_STRING  */
    PREFIX_SUBSTITUTION_COMMAND = 267, /* PREFIX_SUBSTITUTION_COMMAND  */
    PREFIX_SUBSTITUTION_ARITHMETIC = 268, /* PREFIX_SUBSTITUTION_ARITHMETIC  */
    IDENTIFIER_ASSIGNMENT = 269,   /* IDENTIFIER_ASSIGNMENT  */
    STRING = 270,                  /* STRING  */
    STRING_LIST = 271,             /* STRING_LIST  */
    SEMICOLON = 272,               /* SEMICOLON  */
    SEMICOLON_DOUBLE = 273,        /* SEMICOLON_DOUBLE  */
    NEWLINE = 274,                 /* NEWLINE  */
    PIPE = 275,                    /* PIPE  */
    AMPERSAND = 276,               /* AMPERSAND  */
    CODEBLOCK_BEGIN = 277,         /* CODEBLOCK_BEGIN  */
    CODEBLOCK_END = 278,           /* CODEBLOCK_END  */
    LESS = 279,                    /* LESS  */
    GREATER = 280,                 /* GREATER  */
    GREATER_DOUBLE = 281,          /* GREATER_DOUBLE  */
    AND_GREATER = 282,             /* AND_GREATER  */
    GREATER_AND = 283,             /* GREATER_AND  */
    AND_GREATER_DOUBLE = 284,      /* AND_GREATER_DOUBLE  */
    LESS_AND = 285,                /* LESS_AND  */
    NUM_LESS = 286,                /* NUM_LESS  */
    NUM_GREATER = 287,             /* NUM_GREATER  */
    NUM_LESS_AND = 288,            /* NUM_LESS_AND  */
    NUM_GREATER_AND = 289,         /* NUM_GREATER_AND  */
    PLUS = 290,                    /* PLUS  */
    MINUS = 291,                   /* MINUS  */
    TIMES = 292,                   /* TIMES  */
    DIVIDE = 293,                  /* DIVIDE  */
    BRACE_LEFT = 294,              /* BRACE_LEFT  */
    BRACE_RIGHT = 295,             /* BRACE_RIGHT  */
    DOUBLE_QUOTE = 296,            /* DOUBLE_QUOTE  */
    WHITESPACE = 297               /* WHITESPACE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 27 "yar_parser.y"

    string str;
    string_fragment str_frag;
    redirection redirection;
    string_list *str_list;
    argument_list *argument_list;
    command *command;
    process *process;
    job *job;

#line 127 "yar_parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif




#ifndef YYPUSH_MORE_DEFINED
# define YYPUSH_MORE_DEFINED
enum { YYPUSH_MORE = 4 };
#endif

typedef struct yypstate yypstate;


int yypush_parse (yypstate *ps,
                  int pushed_char, YYSTYPE const *pushed_val);

yypstate *yypstate_new (void);
void yypstate_delete (yypstate *ps);


#endif /* !YY_YY_YAR_PARSER_TAB_H_INCLUDED  */
