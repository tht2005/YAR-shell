#include "yar_ast.h"

#include "data_structure/string.h"
#include "yar_debug.h"

string_fragment make_string_fragment (int type, string value)
{
  string_fragment ret;
  ret.type = type;
  ret.value = new_string_2(value);
  return ret;
}

string_fragment_list *new_string_fragment_list (string_fragment str_frag)
{
  string_fragment_list *list = (string_fragment_list *) malloc (sizeof (string_fragment_list));
  if (list == NULL) {
    perror ("Yar: malloc");
    exit (1);
  }
  list->str_frag = str_frag;
  list->next = NULL;
  return list;
}
void free_string_fragment_list (string_fragment_list *list)
{
    for (string_fragment_list *ptr = list, *nxt; ptr; )
    {
        nxt = ptr->next;
        free_string (ptr->str_frag.value);
        free (ptr);
        ptr = nxt;
    }
}

redirection make_redirection (int type, string file)
{
  redirection ret;
  ret.type = type;
  ret.file = file;
  return ret;
}
void free_redirection (redirection redirection)
{
  free_string (redirection.file);
}

string_list *new_string_list (string str)
{
  string_list *list = (string_list *) malloc (sizeof (string_list));
  if (list == NULL) {
    perror ("Yar: malloc");
    exit (1);
  }
  list->str = str;
  list->next = NULL;
  return list;
}
void free_string_list (string_list *ptr)
{
  string_list *nxt;
  while (ptr)
  {
    nxt = ptr->next;
    free_string(ptr->str);
    free (ptr);
    ptr = nxt;
  }
}
size_t string_list_count (string_list *list)
{
  size_t res = 0;
  for(; list; list = list->next)
    ++res;
  return res;
}
string string_list_retrieve_string (string_list *list)
{
  string res = new_string();
  for (; list; list = list->next)
  {
    res = string_append_back (res, list->str);
    if (list->next)
    {
      res = string_push_back (res, ' ');
    }
  }
  return res;
}
void string_list_push_back (string_list **head, string_list **tail, string *str)
{
    string_list *elem = new_string_list (*str);
    if (*head) {
        (*tail)->next = elem;
        (*tail) = elem;
    }
    else {
        (*head) = (*tail) = elem;
    }
    *str = NULL;
}

// string_fragment_list_chain *new_string_fragment_list_chain (string_fragment_list *list)
// {
//   string_fragment_list_chain *chain = (string_fragment_list_chain *) malloc (sizeof (string_fragment_list_chain));
//   if (chain == NULL) {
//     perror ("Yar: malloc");
//     exit (1);
//   }
//   chain->list_head = list;
//   chain->next = NULL;
//   return chain;
// }
// void free_string_fragment_list_chain (string_fragment_list_chain *chain)
// {
//   string_fragment_list_chain *tmp;
//   while (chain)
//   {
//     free_string_fragment_list (chain->list_head);
//     tmp = chain->next;
//     free (chain);
//     chain = tmp;
//   }
// }

void string_fragment_list_push_back (string_fragment_list **head, string_fragment_list **tail, string_fragment *frag)
{
    string_fragment_list *tmp = new_string_fragment_list (*frag);
    if ((*tail) == NULL) {
        (*head) = (*tail) = tmp;
    }
    else {
        (*tail)->next = tmp;
        (*tail) = tmp;
    }
}

// void string_fragment_list_chain_push_back (string_fragment_list_chain **head, string_fragment_list_chain **tail, string_fragment_list **list)
// {
//     string_fragment_list_chain *tmp = new_string_fragment_list_chain (*list);
//     if ((*tail) == NULL) {
//         (*head) = (*tail) = tmp;
//     }
//     else {
//         (*tail)->next = tmp;
//         (*tail) = tmp;
//     }
// }

argument_list *new_argument_list ()
{
  argument_list *list = (argument_list *) malloc (sizeof (argument_list));
  if (list == NULL) {
    perror ("Yar: malloc");
    exit (1);
  }
  list->next = NULL;
  return list;
}
void free_argument_list (argument_list *list)
{
  argument_list *tmp;
  while (list)
  {
    switch (list->type)
    {
      case AL_ARGUMENT:
        free_string (list->arg);
        break;
      case AL_REDIRECTION:
        free_redirection (list->redirection);
        break;
      default:
        assert (0);
    }

    tmp = list->next;
    free (list);
    list = tmp;
  }
}

command *new_command (string_list *assignments, argument_list *arguments_and_redirections)
{
  command *cmd = (command *) malloc (sizeof (command));
  if (cmd == NULL) {
    perror ("Yar: malloc");
    exit (1);
  }
  cmd->assignments = assignments;
  cmd->arguments_and_redirections = arguments_and_redirections;
  DEBUG_PRINT("new_command: %p %p %p\n", cmd, cmd->assignments, cmd->arguments_and_redirections);
  return cmd;
}
void free_command (command *command)
{
  free_string_list (command->assignments);
  free_argument_list (command->arguments_and_redirections);
  free (command);
}
