#include "yar.h"
#include "yar_lexer.h"

#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

int hadError = 0;
int hadRuntimeError = 0;

list_head *list = NULL;

void runtime_error (char *message, token tok) {
    fprintf (stderr, "%s\n[line %d]\n", message, tok.line);
    hadRuntimeError = 1;
}
void report (int line, char *where, char *message) {
    fprintf (stderr, "[line %d] Error %s: %s\n", line, where, message);
    hadError = 1;
}
void error (int line, char *message) {
    report (line, "", message);
}

void run (char *source) {
    list = scan_tokens (source);
    list_head *ptr, *tmp;
    list_for_each_safe (ptr, tmp, list) {
        token_list *obj = list_entry(ptr, token_list, link_node);
        printf ("token: %d %s\n", obj->tok.type, obj->tok.lexeme);
    }
    free_token_list (list);
}

void run_file (char *fp) {
    FILE *file = fopen (fp, "rb");
    if(fp == NULL) {
        perror (fp);
        exit (1);
    }
    fseek (file, 0L, SEEK_END);
    size_t lsize = ftell (file);
    rewind (file);

    char *buffer = calloc (sizeof (char), lsize + 1);
    if (buffer == NULL) {
        fclose (file);
        fprintf (stderr, "memory alloc fails.\n");
        exit (1);
    }

    if (1 != fread (buffer, lsize, 1, file)) {
        fclose (file);
        free (buffer);
        fprintf (stderr, "entire file read fails\n");
        exit (1);
    }

    fclose (file);
    run (buffer);
    free (buffer);

    if (hadError) exit (65);
    if (hadRuntimeError) exit (70);
}

void run_prompt () {
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

        run (buffer);
        hadError = 0;
    }
}

int main(int argc, char *argv[]) {

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

