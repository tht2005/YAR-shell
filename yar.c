#include "yar_lexer.h"

#include <stdio.h>
#include <unistd.h>
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
    char ps[128] = "",
         *p = getenv ("USER"),
         *host = getenv ("HOSTNAME"),
         *s = NULL;
    sprintf (ps, "%s@%s> ", p, host);

    while ((s = readline (ps))) {
        if (strcmp (s, "exit") == 0) {
            free (s);
            break;
        }

        list_head *list = scan_tokens (s);
        list_head *ptr, *tmp;
        list_for_each_safe (ptr, tmp, list) {
            token_list *obj = list_entry(ptr, token_list, link_node);
            printf ("token: %d %s\n", obj->tok.type, obj->tok.lexeme);
        }

        free_token_list (list);

        free (s);
    }

    return 0;
}

