#include "builtin_commands.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "../yar_interpreter.h"

void source_pre ()
{
    // khoi tao?
}

void source_post ()
{
    // giai phong bo nho?
}

int source_main(int argc, char **argv)
{
    if (argc <= 1 || strcmp (argv[1], "--help") == 0) {
        puts ("Usage: source [file]");
        return 0;
    }

    FILE *file = fopen (argv[1], "r");
    if (file == NULL) {
        fprintf (stderr, "Failed to open file.\n");
        return -1;
    }

    char line[6969];
    while (fgets (line, sizeof (line), file)) {
        int len = strlen (line);
        if (len && line[len - 1] == '\n')
            line[--len] = '\0';
        interpret (line);
    }

    return 0;
}

__attribute__((constructor))
static void __source_register (void)
{
    builtin_command_register("source", source_main, source_pre, source_post);
}

