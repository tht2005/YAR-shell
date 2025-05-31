#include "builtin_commands.h"
#include <argp.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void pwd_pre ()
{
    // khoi tao?
}

void pwd_post ()
{
    // giai phong bo nho?
}

int pwd_main(int argc, char **argv)
{
    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0) {
            printf("Usage: pwd\n");
            printf("Print the name of the current working directory.\n\n");
            fflush(stdout);
            return 0;
        }
    }

    char *pwd = getenv ("PWD");
    printf ("%s\n", pwd);
    fflush (stdout);
    return 0;
}

__attribute__((constructor))
static void __pwd_register (void)
{
    builtin_command_register("pwd", SAME_PROCESS, pwd_main, pwd_pre, pwd_post);
}

