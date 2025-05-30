#ifndef __YAR_BUILTIN_COMMANDS_H
#define __YAR_BUILTIN_COMMANDS_H

typedef int (*builtin_func_t)(int argc, char **argv);
typedef void (*callback_t) ();

typedef enum {

  SAME_PROCESS,
  CHILD_PROCESS,

} builtin_type;

typedef struct builtin_command {

  const char *name;
  builtin_type type;
  builtin_func_t func_main;

} builtin_command;

void builtin_command_register (const char *name, builtin_type type, builtin_func_t func_main, callback_t func_pre, callback_t func_post);
int exec_builtin (const char *name, int argc, char **argv);

#endif 

