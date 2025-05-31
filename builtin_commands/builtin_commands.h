#ifndef __YAR_BUILTIN_COMMANDS_H
#define __YAR_BUILTIN_COMMANDS_H

typedef int (*builtin_func_t)(int argc, char **argv);
typedef void (*callback_t) ();

typedef enum {

  SAME_PROCESS,
  CHILD_PROCESS,

} builtin_type_t;

typedef struct builtin_command_t {

  const char *name;
  builtin_type_t type;
  builtin_func_t func_main;

} builtin_command_t;

void builtin_command_register (const char *name, builtin_type_t type, builtin_func_t func_main, callback_t func_pre, callback_t func_post);

#define COMMAND_FOUND_NOT_EXECUTABLE    126
#define COMMAND_NOT_FOUND               127

int exec_builtin (int argc, char **argv);

#endif 

