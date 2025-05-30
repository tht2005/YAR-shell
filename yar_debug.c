#include "yar_debug.h"
#include "yar_ast.h"

#include <assert.h>

void debug_string_list (string_list *list)
{
    DEBUG_PRINT("debug: begin string list\n");
    for (; list; list = list->next)
    {
        DEBUG_PRINT("`%s`\n", list->str);
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
