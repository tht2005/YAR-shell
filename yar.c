#include "yar.h"
#include "data_structure/string.h"
#include "yar_env.h"
#include "yar_interpreter.h"
#include "yar_shell.h"

#include <string.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <readline/readline.h>
#include <readline/history.h>

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

string read_multiline_input (const char *prompt)
{
    char *line = NULL;
    string fullline = NULL;

    while (1)
    {
        line = readline(prompt);
        if (got_sigint)
        {
            if (line) free (line);
            if (fullline) free_string (fullline);
            return NULL;
        }
        if (line == NULL)
        {
            break;
        }

        size_t len = strlen(line);
        int cont = (len > 0 && line[len - 1] == '\\');
        if (cont)
        {
            line[len - 1] = '\0';
        }

        fullline = string_append_back(fullline, line);

        free (line);

        if (!cont) {
            break;
        }

        prompt = "> ";
    }
    return fullline;
}

void run_prompt () {
    putenv ("HOSTNAME=Gentoo");

    char ps[128] = "";
    char *user = getenv("USER");
    char *hostname = getenv("HOSTNAME");

    string input;
    while (1) {
        char *cwd = getcwd (NULL, 0);
        snprintf (ps, 100, "%s@%s %s $ ", user, hostname, cwd);
        free (cwd);

        input = read_multiline_input(ps);
        if (input == NULL) {
            if (got_sigint) {
                got_sigint = 0;
                continue;
            }
            break;
        }

        if (*input) {
            add_history (input);
        }
        interpret (input);
        free_string (input);
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

