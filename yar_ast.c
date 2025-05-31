#include "yar_ast.h"

#include "data_structure/string.h"

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

