#include "builtin_commands.h"

#include <stdio.h>

void help_pre ()
{
    // khoi tao?
}

void help_post ()
{
    // giai phong bo nho?
}

int help_main(int argc, char **argv)
{

    printf("Builtin Commands:\n");
    printf("  help                  Show this help message\n");
    printf("  exit [n]              Exit the shell with exit code n\n");
    printf("  date: [format]        Display current date/time in given format\n");
    printf("  ls [path]             List contents of the directory\n");
    printf("  path                  Show environment paths\n");
    printf("  path -a [path]        Add a path to environment paths\n");
    printf("  path -d [index]       Remove path at index from environment paths\n");
    printf("  echo [string,...]     Print the given strings\n");
    printf("  pwd                   Show current working directory\n");
    printf("  source [file]         Execute commands from a file\n");
    printf("  time [command]        Measure execution time of a command\n\n");

    printf("\nJob Control:\n");
    printf("  jobs                  List background jobs\n");
    printf("  fg [pgid]             Resume job in foreground\n");
    printf("  bg [pgid]             Resume job in background\n");
    printf("  suspend [pgid]        Suspend job with given pgid\n");
    printf("  kill [pgid]           Terminate job with given pgid\n\n");
    fflush (stdout);
    return 0;
}

__attribute__((constructor))
static void __help_register (void)
{
    builtin_command_register("help", help_main, help_pre, help_post);
}

