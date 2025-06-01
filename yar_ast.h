#ifndef __YAR_AST_H
#define __YAR_AST_H

#include "data_structure/string.h"

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
size_t string_list_count (string_list *list);
string string_list_retrieve_string (string_list *list);
void string_list_push_back (string_list **head, string_list **tail, string *str);

typedef struct redirection {

  int type;
  string file;

} redirection;

redirection make_redirection (int type, string file);

void string_fragment_list_push_back (string_fragment_list **head, string_fragment_list **tail, string_fragment *frag);
// void string_fragment_list_chain_push_back (string_fragment_list_chain **head, string_fragment_list_chain **tail, string_fragment_list **list);

typedef struct argument_list
{

  enum {
    AL_ARGUMENT,
    AL_REDIRECTION,
  } type;
  union {
    string arg;
    redirection redirection;
  };
  struct argument_list *next;

} argument_list;
argument_list *new_argument_list ();

typedef struct command {

  string_list *assignments;
  argument_list *arguments_and_redirections;

} command;
command *new_command (string_list *assignments, argument_list *arguments_and_redirections);
void free_command (command *command);

#endif
