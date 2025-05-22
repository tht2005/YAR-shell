#ifndef __ENVIRON_H
#define __ENVIRON_H

#include <stdlib.h>

typedef struct environ_t environ_t;

size_t environ_size (environ_t *env);

environ_t *new_environ ();
void free_environ (environ_t *env);

char* getenv_nofree (environ_t *env, char *name);
int putenv_nofree (environ_t *env, char *string);
int setenv_nofree (environ_t *env, char *name, char *value, int replace);
int unsetenv_nofree (environ_t *env, char *name);

#endif

