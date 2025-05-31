#include <ctype.h>
#include <dirent.h>
#include <errno.h>

#include "builtin_commands.h"

#include <stdio.h>
#include <string.h>
#include "../data_structure/string.h"
#include <sys/resource.h>
#include <sys/wait.h>

void exit_pre ()
{
}

void exit_post ()
{
}


int is_number(const char *s) {
    if (*s == '-' || *s == '+') s++;
    if (*s == '\0') return 0;
    while (*s) {
        if (!isdigit((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}

int exit_main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        printf("Usage: exit [N]\n\n");
        printf("Exit the shell with a status of N\n");
        printf("If N is omitted, the exit status is 0.\n\n");

        fflush(stdout);
        return 0;
    }

    if (argc == 1) {
        exit(0);
    }

    if (argc > 1) {
        if (is_number(argv[1])) {
            exit(atoi(argv[1]));
        } else {
            fprintf(stderr, "exit: %s: numeric argument required\n", argv[1]);
            exit(2);
        }
    }

    fflush (stdout);
    return 0;
}

__attribute__((constructor))
static void __exit_register (void)
{
    builtin_command_register("exit", exit_main, exit_pre, exit_post);
}

