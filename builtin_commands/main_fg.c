#include "builtin_commands.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "../yar_job.h"

void fg_pre ()
{
    // khoi tao?
}

void fg_post ()
{
    // giai phong bo nho?
}

int fg_main(int argc, char **argv)
{
    if (argc <= 1) {
        puts ("Usage: fg [--help | pgid]");
        return -1;
    }
    if (argc > 2) {
        puts ("Usage: fg [--help | pgid]");
        puts ("To much arguments.");
        return -1;
    }
    if (strcmp(argv[1], "--help") == 0) {
        puts ("Usage: fg [--help | pgid]");
        puts ("Continue a job and put it in foreground mode.");
        return 0;
    } 

    int pgid;
    if (safe_str_to_int (argv[1], &pgid)) {
        fprintf (stderr, "Not valid pgid!\n");
        return -1;
    }

    job *j = find_job (pgid);
    if (j == NULL) {
        fprintf (stderr, "No such job with pgid=%d.\n", pgid);
        return -2;
    }

    fprintf (stderr, "%s\n", j->command);
    j->foreground = 1;
    continue_job (j);

    return 0;
}

__attribute__((constructor))
static void __fg_register (void)
{
    builtin_command_register("fg", fg_main, fg_pre, fg_post);
}

