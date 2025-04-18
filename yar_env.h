#ifndef __YAR_ENV_H
#define __YAR_ENV_H

struct scope_environ_t;
typedef struct scope_environ_t scope_environ_t;

void init_env ();

int yar_push_env ();
int yar_pop_env ();

int yar_setenv (const char *, const char *, int);
int yar_unsetenv (const char *);
char* yar_getenv (const char *name);

char **yar_calculate_environ ();
char **yar_calculate_global_environ ();

#endif
