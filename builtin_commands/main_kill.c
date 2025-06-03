#include "builtin_commands.h"

#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "../yar_job.h"

void kill_pre ()
{
    // khoi tao?
}

void kill_post ()
{
    // giai phong bo nho?
}

int kill_main(int argc, char **argv)
{
    if (argc <= 1 || argc > 3) {
        puts ("Usage: kill --help");
        puts ("       kill pgid");
        puts ("       kill -f pgid");
        return -1;
    }
    if (strcmp(argv[1], "--help") == 0) {
        puts ("Usage: kill --help");
        puts ("       kill pgid");
        puts ("       kill -f pgid");
        puts ("Terminate a job.");
        return 0;
    } 

    int integer_slot = 1;
    if (argc >= 3)
        integer_slot = 2;

    int force = 0;
    if (argc >= 3 && strcmp (argv[1], "-f") == 0) {
        force = 1;
    }

    int pgid;
    if (safe_str_to_int (argv[integer_slot], &pgid)) {
        fprintf (stderr, "Not valid pgid!\n");
        return -1;
    }

    job *j = find_job (pgid);
    if (j == NULL) {
        fprintf (stderr, "No such job with pgid=%d.\n", pgid);
        return -2;
    }

    kill (-j->pgid,
            force ? SIGKILL : SIGTERM);
    return 0;
}

__attribute__((constructor))
static void __kill_register (void)
{
    builtin_command_register("kill", kill_main, kill_pre, kill_post);
}

