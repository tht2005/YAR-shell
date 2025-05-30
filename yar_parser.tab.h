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
    #include "data_structure/string.h"

#line 54 "yar_parser.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    PREFIX_PROGRAM_SEGMENT = 258,  /* PREFIX_PROGRAM_SEGMENT  */
    PREFIX_FUNCTION = 259,         /* PREFIX_FUNCTION  */
    PREFIX_CODEBLOCK = 260,        /* PREFIX_CODEBLOCK  */
    PREFIX_ASSIGNMENT = 261,       /* PREFIX_ASSIGNMENT  */
    PREFIX_REDIRECTION = 262,      /* PREFIX_REDIRECTION  */
    PREFIX_COMMAND = 263,          /* PREFIX_COMMAND  */
    PREFIX_SUBSTITUTION_STRING = 264, /* PREFIX_SUBSTITUTION_STRING  */
    PREFIX_SUBSTITUTION_COMMAND = 265, /* PREFIX_SUBSTITUTION_COMMAND  */
    PREFIX_SUBSTITUTION_ARITHMETIC = 266, /* PREFIX_SUBSTITUTION_ARITHMETIC  */
    IDENTIFIER_ASSIGNMENT = 267,   /* IDENTIFIER_ASSIGNMENT  */
    STRING = 268,                  /* STRING  */
    SEMICOLON = 269,               /* SEMICOLON  */
    SEMICOLON_DOUBLE = 270,        /* SEMICOLON_DOUBLE  */
    NEWLINE = 271,                 /* NEWLINE  */
    LESS = 272,                    /* LESS  */
    GREATER = 273,                 /* GREATER  */
    GREATER_DOUBLE = 274,          /* GREATER_DOUBLE  */
    AND_GREATER = 275,             /* AND_GREATER  */
    GREATER_AND = 276,             /* GREATER_AND  */
    AND_GREATER_DOUBLE = 277,      /* AND_GREATER_DOUBLE  */
    LESS_AND = 278,                /* LESS_AND  */
    NUM_LESS = 279,                /* NUM_LESS  */
    NUM_GREATER = 280,             /* NUM_GREATER  */
    NUM_LESS_AND = 281,            /* NUM_LESS_AND  */
    NUM_GREATER_AND = 282,         /* NUM_GREATER_AND  */
    PLUS = 283,                    /* PLUS  */
    MINUS = 284,                   /* MINUS  */
    TIMES = 285,                   /* TIMES  */
    DIVIDE = 286,                  /* DIVIDE  */
    WHITESPACE = 287               /* WHITESPACE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 20 "yar_parser.y"

    string str;
    string_fragment str_frag;
    redirection redirection;
    struct {
        string_fragment_list *head, *tail;
    } fragment_list;

#line 112 "yar_parser.tab.h"

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
