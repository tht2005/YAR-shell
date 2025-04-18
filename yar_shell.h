#ifndef __YAR_SHELL_H
#define __YAR_SHELL_H

#include <sys/types.h>
#include <termios.h>

extern int shell_terminal;
extern int shell_is_interactive;
extern pid_t shell_pgid;
extern struct termios shell_tmodes;

void init_shell ();

#endif
