#include "yar_ast.h"

#include "data_structure/string.h"

string_fragment make_string_fragment (int quoted, string value)
{
  string_fragment ret;
  ret.quoted = quoted;
  ret.value = value;
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

