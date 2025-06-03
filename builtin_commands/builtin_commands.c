#include "builtin_commands.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int safe_str_to_int(const char *str, int *out) {
    char *endptr;
    long val;

    errno = 0;
    val = strtol(str, &endptr, 10);  // base 10

    if (endptr == str) {
        return -1;
    }

    if (*endptr != '\0') {
        return -2;
    }

    if ((errno == ERANGE) || val < INT_MIN || val > INT_MAX) {
        return -3;
    }

    *out = (int)val;
    return 0;
}

#define MAX_BUILTIN_COMMAND         50

size_t cnt_command = 0;
builtin_command_t cmd[MAX_BUILTIN_COMMAND]; 

builtin_command_t make_builtin_command (const char *name, builtin_func_t func_main)
{
    builtin_command_t ret;
    ret.name = name;
    ret.func_main = func_main;
    return ret;
}

void builtin_command_register(const char *name,
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
    cmd[cnt_command++] = make_builtin_command (name, func_main);
    //fprintf (stderr, "Builtin command `%s` registered!\n", name);
}

int check_builtin (const char *name)
{
    for (size_t i = 0; i < cnt_command; ++i)
    {
        if (strcmp (name, cmd[i].name) == 0) {
            return 1;
        }
    }
    return 0;
}
int exec_builtin (int argc, char **argv)
{
    if (argc == 0) {
        return -1;
    }
    for (size_t i = 0; i < cnt_command; ++i)
    {
        if (strcmp (argv[0], cmd[i].name) == 0) {
            return cmd[i].func_main (argc, argv);
        }
    }
    return COMMAND_NOT_FOUND;
}

