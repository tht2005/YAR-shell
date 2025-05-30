#include "yar.h"
#include "yar_env.h"
#include "yar_interpreter.h"
#include "yar_shell.h"

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "data_structure/string.h"

// void run_file (char *fp) {
//     FILE *file = fopen (fp, "rb");
//     if(fp == NULL) {
//         perror (fp);
//         exit (1);
//     }
//     fseek (file, 0L, SEEK_END);
//     size_t lsize = ftell (file);
//     rewind (file);
//
//     char *buffer = calloc (sizeof (char), lsize + 1);
//     if (buffer == NULL) {
//         fclose (file);
//         fprintf (stderr, "memory alloc fails.\n");
//         exit (1);
//     }
//
//     if (1 != fread (buffer, lsize, 1, file)) {
//         fclose (file);
//         free (buffer);
//         fprintf (stderr, "entire file read fails\n");
//         exit (1);
//     }
//
//     fclose (file);
//     run (buffer);
//     free (buffer);
//
//     if (hadError) exit (65);
//     if (hadRuntimeError) exit (70);
// }

void run_prompt () {
    putenv ("HOSTNAME=Gentoo");

    char *input;
    char ps[128] = "";
    char *user = getenv("USER");
    char *hostname = getenv("HOSTNAME");
    char *cwd = getcwd (NULL, 0);

    snprintf (ps, 100, "%s@%s %s $ ", user, hostname, cwd);
    while ( (input = readline (ps)) ) {
        if (*input) {
            add_history (input);
        }
        interpret (input);
        free (input);
    }
}


int main(int argc, char *argv[]) {

    init_env ();
    init_shell ();

    if (argc > 2) {
        puts ("Usage: yar");
        puts ("       yar [source file]");
        exit (64);
    }
    else if (argc == 2) {
        // run_file (argv[1]);
    }
    else {
        run_prompt ();
    }

    return 0;
}

