#ifndef __YAR_AST_H
#define __YAR_AST_H

#include "data_structure/string.h"

typedef struct brace_expansion_t {

  enum {

    BRACE_RANGE,
    BRACE_LIST,

  } type;

  union {

  };

} brace_expansion_t;

typedef struct string_fragment {

  enum {

    STR_FRAG_QUOTED,
    STR_FRAG_NON_QUOTED,

  } type;

  string value;

} string_fragment;

string_fragment make_string_fragment (int quoted, string value);

typedef struct string_fragment_list {

  string_fragment str_frag;
  struct string_fragment_list *next;

} string_fragment_list;

string_fragment_list *new_string_fragment_list (string_fragment str_frag);
void free_string_fragment_list (string_fragment_list *list);

// typedef struct string_fragment_list_chain {
//
//   string_fragment_list *list_head;
//   struct string_fragment_list_chain *next;
//
// } string_fragment_list_chain;
//
// string_fragment_list_chain *new_string_fragment_list_chain (string_fragment_list *list);

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

void string_list_push_back (string_list **head, string_list **tail, string *str);
void string_fragment_list_push_back (string_fragment_list **head, string_fragment_list **tail, string_fragment *frag);
// void string_fragment_list_chain_push_back (string_fragment_list_chain **head, string_fragment_list_chain **tail, string_fragment_list **list);

#endif
