#ifndef __YAR_BUILTIN_COMMANDS_H
#define __YAR_BUILTIN_COMMANDS_H

typedef int (*builtin_func_t)(int argc, char **argv);
typedef void (*callback_t) ();

void builtin_command_register (const char *name, builtin_func_t func_main, callback_t func_pre, callback_t func_post);

#endif 

