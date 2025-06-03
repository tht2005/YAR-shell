#include "builtin_commands.h"

#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "../yar_job.h"

void suspend_pre ()
{
    // khoi tao?
}

void suspend_post ()
{
    // giai phong bo nho?
}

int suspend_main(int argc, char **argv)
{
    if (argc <= 1) {
        puts ("Usage: suspend [--help | pgid]");
        return -1;
    }
    if (argc > 2) {
        puts ("Usage: suspend [--help | pgid]");
        puts ("To much arguments.");
        return -1;
    }
    if (strcmp(argv[1], "--help") == 0) {
        puts ("Usage: suspend [--help | pgid]");
        puts ("Stop a job."); 
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
    kill (-j->pgid, SIGSTOP);
    return 0;
}

__attribute__((constructor))
static void __suspend_register (void)
{
    builtin_command_register("suspend", suspend_main, suspend_pre, suspend_post);
}

