#include "builtin_commands.h"
#include <argp.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void echo_pre ()
{
    // khoi tao?
}

void echo_post ()
{
    // giai phong bo nho?
}

int echo_main(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '$' && strlen(argv[i]) > 1) {
            char *var = getenv(argv[i] + 1);
            if (var != NULL) {
                printf("%s", var);
            }
        }
        else {
            printf("%s", argv[i]);
        }
        if (i < argc - 1) {
            printf(" ");
        }
    }

    fflush (stdout);
    return 0;
}

__attribute__((constructor))
static void __echo_register (void)
{
    builtin_command_register("echo", echo_main, echo_pre, echo_post);
}

