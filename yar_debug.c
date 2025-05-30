#include "yar_debug.h"
#include "yar_ast.h"

#include <assert.h>

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
            case STR_FRAG_BRACE:
                break;
            default:
                assert (0);
                break;
        }
    }
    DEBUG_PRINT("<end>\n");
}
