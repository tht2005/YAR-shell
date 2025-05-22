#include "yar_env.h"
#include "data_structure/environ.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct scope_environ_t {
    struct scope_environ_t *parent;
    environ_t *env;
} scope_environ_t;

void __free_scope_environ_t (scope_environ_t *e)
{
    free_environ (e->env);
    free (e);
}

scope_environ_t *global_current;

void __exit_env () {

}

// char ** __new_array_of_char_ast () {
//     char **a = (char **) malloc (sizeof (char *) << 2);
//     if (a == NULL) {
//         fprintf (stderr, "Yar: __new_array_of_char_ast allocation failed!\n");
//         return NULL;
//     }
//     a[0] = NULL;
//     return a;
// }

void init_env () {

    global_current = yar_push_env (NULL);

    atexit (__exit_env);
}

scope_environ_t* yar_push_env (scope_environ_t *current) {
    scope_environ_t *new_scope = (scope_environ_t *) malloc (sizeof (scope_environ_t));
    if (new_scope == NULL) {
        fprintf(stderr, "Yar: Memory allocation failed for new scope!\n");
        return NULL;
    }
    new_scope-> parent = current;
    new_scope->env = new_environ ();
    return new_scope;
}

scope_environ_t * yar_pop_env (scope_environ_t *current, int delete) {
    if (current == NULL) {
        fprintf (stderr, "Yar: (DEV CRITICAL BUG) current is NULL!\n");
        return NULL;
    }
    if (current->parent == NULL) {
        fprintf (stderr, "Yar: (DEV CRITICAL BUG) current->parent is NULL!\n");
        return NULL;
    }
    scope_environ_t *parent = current->parent;
    if (delete) {
        __free_scope_environ_t (current);
    }
    return parent;
}

