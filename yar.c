#include "yar.h"
#include "yar_env.h"
#include "yar_parser.h"
#include "yar_shell.h"

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "data_structure/string.h"

void run (char *source) {
    init_parser (source);
    test ();
}

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
    // interactive mode

    // yar_setenv ("HOSTNAME", "Gentoo", 1);

    // can't use valgrind with readline lib so ...
    
    // while ((s = readline (ps))) {
    //     if (strcmp (s, "exit") == 0) {
    //         free (s);
    //         break;
    //     }
    //
    //     list_head *list = scan_tokens (s);
    //     list_head *ptr, *tmp;
    //     list_for_each_safe (ptr, tmp, list) {
    //         token_list *obj = list_entry(ptr, token_list, link_node);
    //         printf ("token: %d %s\n", obj->tok.type, obj->tok.lexeme);
    //     }
    //
    //     free_token_list (list);
    //
    //     free (s);
    // }

    while (1) {

        char ps[128] = "",
             *p = NULL,
             *host = NULL,
             *s = NULL;
             // *p = yar_getenv ("USER"),
             // *host = yar_getenv ("HOSTNAME"),
             // *s = NULL;

        char *cwd = getcwd (NULL, 0);
        snprintf (ps, 100, "%s@%s %s $ ", p, host, cwd);

        free (cwd);

        printf ("%s", ps);
        char buffer[1000];
        fgets (buffer, 800, stdin);
        int len = strlen (buffer);

        if (strncmp (buffer, "exit", 4) == 0) {
            break;
        }

        run (buffer);
        // reset error
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

