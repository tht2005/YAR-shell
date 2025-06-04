#include "builtin_commands.h"

#include <bits/time.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../yar_interpreter.h"

void time_pre ()
{
    // khoi tao?
}

void time_post ()
{
    // giai phong bo nho?
}

int time_main(int argc, char **argv)
{
    if (argc <= 1) {
        puts ("Usage: time [--help | <command>]");
        puts ("Two few arguments");
        return -1;
    }
    if (strcmp(argv[1], "--help") == 0) {
        puts ("Usage: time [--help | pgid]");
        puts ("Calculate command running time."); 
        return 0;
    } 

    string input = new_string ();
    for (int i = 1; i < argc; ++i) {
        input = string_append_back (input, argv[i]);
        input = string_push_back (input, ' ');
    }

    struct timespec begin, end;
    clock_gettime (CLOCK_MONOTONIC, &begin);
    interpret (input);
    clock_gettime (CLOCK_MONOTONIC, &end);
    free_string (input);

    double elapsed = (end.tv_sec - begin.tv_sec)
                + (end.tv_nsec - begin.tv_nsec) / 1e9;

    printf ("Elapsed time: %.6f second\n", elapsed);

    return 0;
}

__attribute__((constructor))
static void __time_register (void)
{
    builtin_command_register("time", time_main, time_pre, time_post);
}

