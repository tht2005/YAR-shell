#include "yar_env.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

extern char **environ;
static pthread_mutex_t environ_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct scope_environ_t {
    struct scope_environ_t *parent;
    char **environ;
} scope_environ_t;

scope_environ_t global;
scope_environ_t *current;

void __exit_env () {
    yar_pop_env ();

    if (pthread_mutex_destroy (&environ_mutex) != 0) {
        fprintf(stderr, "Yar: mutex destroy failed!\n");
        exit(EXIT_FAILURE);
    }
}

void init_env () {
    global.parent = NULL;
    global.environ = environ;
    current = &global;

    yar_push_env ();

    atexit (__exit_env);
}

int yar_push_env () {
    scope_environ_t *new_scope = (scope_environ_t *) malloc (sizeof (scope_environ_t));
    if (new_scope == NULL) {
        fprintf(stderr, "Yar: Memory allocation failed for new scope!\n");
        return -1;
    }
    new_scope-> parent = current;
    new_scope->environ = NULL;
    current = new_scope; 
    return 0;
}

int yar_pop_env () {
    if (current == NULL) {
        fprintf (stderr, "Yar: (DEV CRITICAL BUG) current is NULL!\n");
        return -1;
    }
    if (current->parent == NULL) {
        fprintf (stderr, "Yar: (DEV CRITICAL BUG) current->parent is NULL!\n");
        return -1;
    }
    scope_environ_t *parent = current->parent;
    free (current);
    current = parent;
    return 0;
}

int yar_setenv (const char *name, const char *value, int replace) {
    int status;
    char **old_environ;

    if (pthread_mutex_lock (&environ_mutex) != 0) {
        fprintf (stderr, "Yar: environt mutex lock failed!\n");
        return -1;
    }

    old_environ = environ;
    environ = current->environ;

    status = setenv (name, value, replace);

    current->environ = environ;
    environ = old_environ;

    if (pthread_mutex_unlock (&environ_mutex) != 0) {
        fprintf (stderr, "Yar: environt mutex unlock failed!\n");
        return -1;
    }

    return status;
}

// int yar_unsetenv (const char* name) {
//     int status;
//     char **old_environ;
//
//     if (pthread_mutex_lock (&environ_mutex) != 0) {
//         fprintf (stderr, "Yar: environt mutex lock failed!\n");
//         exit (EXIT_FAILURE);
//     }
//
//     old_environ = environ;
//     if (current->environ == NULL) {
//         fprintf (stderr, "Yar: (DEV CRITICAL BUG) current->environ not set!\n");
//         return -1;
//     }
//     environ = current->environ;
//     status = unsetenv (name);
//     environ = old_environ;
//
//     if (pthread_mutex_unlock (&environ_mutex) != 0) {
//         fprintf (stderr, "Yar: environt mutex unlock failed!\n");
//         exit (EXIT_FAILURE);
//     }
//
//     return status;
// }

char *yar_getenv (const char *name) {
    char **old_environ;
    char *result = NULL;

    if (pthread_mutex_lock (&environ_mutex) != 0) {
        fprintf (stderr, "Yar: environt mutex lock failed!\n");
        return NULL;
    }

    old_environ = environ;
    for (scope_environ_t *p = current; p != NULL && result == NULL; p = p->parent) {
        environ = p->environ;
        result = getenv (name);
    }
    environ = old_environ;

    if (pthread_mutex_unlock (&environ_mutex) != 0) {
        fprintf (stderr, "Yar: environt mutex unlock failed!\n");
        return NULL;
    }

    return result ? result : "";
}

