#include "yar.h"
#include "data_structure/string.h"
#include "yar_env.h"
#include "yar_interpreter.h"
#include "yar_job.h"
#include "yar_shell.h"

#include <string.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <readline/readline.h>
#include <readline/history.h>

void run_file (char *fp) {
    FILE *file = fopen (fp, "r");
    if (file == NULL) {
        fprintf (stderr, "Failed to open file.\n");
        exit (-1);
    }
    char line[6969];
    while (fgets (line, sizeof (line), file)) {
        int len = strlen (line);
        if (len && line[len - 1] == '\n')
            line[--len] = '\0';
        if (*line && line[0] != '#')
            interpret (line);
    }
    fclose (file);
    exit (0);
}

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

    char ps[128] = "";
    char *user = getenv("USER");
    char *hostname = getenv("HOSTNAME");

    string input;
    while (1) {
        char *cwd = getcwd (NULL, 0);
        snprintf (ps, 100, "tiny-shell %s $ ", cwd);
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

        if (shell_is_interactive)
        {
            do_job_notification();
        }
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
        run_file (argv[1]);
    }
    else {
        run_prompt ();
    }

    return 0;
}

