#include "yar_lexer.h"

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

int main(int argc, char *argv[]) {

    if (argc > 2) {
        puts ("Usage: yar");
        puts ("       yar [source file]");
        exit (64);
    }

    if (argc == 2) {
        // execute
        return 0;
    }

    // interactive mode

    int cont = 1;

    while (cont) {

        char *s = readline ("test> ");

        if (strcmp (s, "exit") == 0) {
            cont = 0;
        }

        free (s);
    }


    return 0;
}

