#ifndef __YAR_ENV_H
#define __YAR_ENV_H

void init_env ();

int yar_push_env ();
int yar_pop_env ();

int yar_setenv (const char *, const char *, int);
int yar_unsetenv (const char *);
char* yar_getenv (const char *name);

#endif
