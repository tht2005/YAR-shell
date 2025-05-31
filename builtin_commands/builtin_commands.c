#include "builtin_commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUILTIN_COMMAND         50

size_t cnt_command = 0;
builtin_command_t cmd[MAX_BUILTIN_COMMAND]; 

builtin_command_t make_builtin_command (const char *name, builtin_type_t type, builtin_func_t func_main)
{
    builtin_command_t ret;
    ret.name = name;
    ret.type = type;
    ret.func_main = func_main;
    return ret;
}

void builtin_command_register(const char *name,
                                builtin_type_t type,
                                builtin_func_t func_main,
                                callback_t func_pre,
                                callback_t func_post)
{
    if (cnt_command >= MAX_BUILTIN_COMMAND)
    {
        fprintf (stderr, "Builtin command `%s` not registered, because number of builtin-commands exceed MAX_BUILTIN_COMMAND.\n", name);
        return ;
    }
    func_pre ();
    atexit (func_post);
    cmd[cnt_command++] = make_builtin_command (name, type, func_main);
    fprintf (stderr, "Builtin command `%s` registered (%s)\n", name, type == SAME_PROCESS ? "same process" : "child process");
}

int exec_builtin (int argc, char **argv)
{
    if (argc == 0) {
        return -1;
    }
    for (size_t i = 0; i < cnt_command; ++i)
    {
        if (strcmp (argv[0], cmd[i].name) == 0) {
            cmd[i].func_main (argc, argv);
            return 0;
        }
    }
    return COMMAND_NOT_FOUND;
}

