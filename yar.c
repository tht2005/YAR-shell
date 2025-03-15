#include "yar_lexer.h"

#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

list_head *list = NULL;

void loop_exit (void) {
    if (list) {
        free_token_list(list);
        list = NULL;
    }
}

int main(int argc, char *argv[]) {

    if (argc > 2) {
        puts ("Usage: yar");
        puts ("       yar [source file]");
        exit (64);
    }

    if (argc == 2) {
        FILE *fp = fopen (argv[1], "rb");
        if(fp == NULL) {
            perror (argv[1]);
            exit (1);
        }
        fseek (fp, 0L, SEEK_END);
        size_t lsize = ftell (fp);
        rewind (fp);

        char *buffer = calloc (sizeof (char), lsize + 1);
        if (buffer == NULL) {
            fclose (fp);
            fprintf (stderr, "memory alloc fails.\n");
            exit (1);
        }

        if (1 != fread (buffer, lsize, 1, fp)) {
            fclose (fp);
            free (buffer);
            fprintf (stderr, "entire file read fails\n");
            exit (1);
        }

        fclose (fp);

        // do my work here

        free (buffer);
        return 0;
    }

    // interactive mode
    char ps[128] = "",
         *p = getenv ("USER"),
         *host = getenv ("HOSTNAME"),
         *s = NULL;
    snprintf (ps, 127, "%s@%s> ", p, host);

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

    atexit (loop_exit);

    while (1) {
        printf ("%s", ps);
        char buffer[1000];
        fgets (buffer, 999, stdin);
        int len = strlen (buffer);
        if(len && buffer[len - 1] == '\n')
            buffer[--len] = '\0';

        if (strcmp (buffer, "exit") == 0) {
            break;
        }

        list = scan_tokens (buffer);
        list_head *ptr, *tmp;
        list_for_each_safe (ptr, tmp, list) {
            token_list *obj = list_entry(ptr, token_list, link_node);
            printf ("token: %d %s\n", obj->tok.type, obj->tok.lexeme);
        }

        loop_exit ();
    }

    return 0;
}

