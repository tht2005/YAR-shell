#include "yar_debug.h"
#include "yar_ast.h"

#include <assert.h>

#include <string.h>

void debug_command (command *command)
{
    if (command->assignments)
    {
        for (string_list *ptr = command->assignments; ptr; ptr = ptr->next)
        {
            DEBUG_PRINT("debug: assignment: `%s`\n", ptr->str);
        }
    }
    if (command->arguments_and_redirections) {
        for (argument_list *ptr = command->arguments_and_redirections; ptr; ptr = ptr->next)
        {
            if (ptr->type == AL_ARGUMENT)
            {
                DEBUG_PRINT("debug: argument: `%s`\n", ptr->arg);
            }
            else
            {
                DEBUG_PRINT("debug: redirection: (%d) `%s`\n", ptr->redirection.type, ptr->redirection.file);
            }
        }
    }
}
void debug_string_list (string_list *list)
{
    DEBUG_PRINT("debug: begin string list\n");
    for (; list; list = list->next)
    {
        DEBUG_PRINT("`%s` (%ld)\n", list->str, strlen (list->str));
    }
    DEBUG_PRINT("debug: end string list\n");
}

void debug_string_fragment_list (string_fragment_list *list)
{
    DEBUG_PRINT("debug: string_fragment_list: <start>");
    for (; list; list = list->next)
    {
        switch(list->str_frag.type)
        {
            case STR_FRAG_QUOTED:
                DEBUG_PRINT("`%s`", list->str_frag.value);
                break;
            case STR_FRAG_NON_QUOTED:
                DEBUG_PRINT("%s", list->str_frag.value);
                break;
            default:
                assert (0);
                break;
        }
    }
    DEBUG_PRINT("<end>\n");
}
