#ifndef __YAR_ENV_H
#define __YAR_ENV_H

struct scope_environ_t;
typedef struct scope_environ_t scope_environ_t;

extern scope_environ_t *global, *global_current;

void init_env ();

scope_environ_t* yar_push_env (scope_environ_t *);
scope_environ_t* yar_pop_env (scope_environ_t *, int);

#endif
