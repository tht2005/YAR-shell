#ifndef __YAR_BUILTIN_COMMANDS_H
#define __YAR_BUILTIN_COMMANDS_H

int safe_str_to_int(const char *str, int *out);

typedef int (*builtin_func_t)(int argc, char **argv);
typedef void (*callback_t) ();

typedef struct builtin_command_t {

  const char *name;
  builtin_func_t func_main;

} builtin_command_t;

void builtin_command_register (const char *name, builtin_func_t func_main, callback_t func_pre, callback_t func_post);

#define COMMAND_FOUND_NOT_EXECUTABLE    126
#define COMMAND_NOT_FOUND               127

int check_builtin (const char *name);
int exec_builtin (int argc, char **argv);

#endif 

