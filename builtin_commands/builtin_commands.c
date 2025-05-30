#include "builtin_commands.h"

#include <stdio.h>

void builtin_command_register(const char *name,
                                builtin_type type,
                                builtin_func_t func_main,
                                callback_t func_pre,
                                callback_t func_post)
{
    fprintf (stderr, "Builtin command `%s` registered (%s)\n", name, type == SAME_PROCESS ? "same process" : "child process");
}
