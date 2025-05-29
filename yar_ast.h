#ifndef __YAR_AST_H
#define __YAR_AST_H

#include "data_structure/string.h"

typedef struct string_fragment {

  int quoted;
  string value;

} string_fragment;

string_fragment make_string_fragment (int quoted, string value);

typedef struct string_fragment_list {

  string_fragment str_frag;
  struct string_fragment_list *next;

} string_fragment_list;

string_fragment_list *new_string_fragment_list (string_fragment str_frag);

typedef struct string_list {

  string str;
  struct string_list *next;

} string_list;

string_list *new_string_list (string str);
void free_string_list (string_list *ptr);

typedef struct redirection {

  int type;
  string file;

} redirection;

redirection make_redirection (int type, string file);

#endif
