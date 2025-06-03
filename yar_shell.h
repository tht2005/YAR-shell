#ifndef __YAR_SHELL_H
#define __YAR_SHELL_H

#include <sys/types.h>
#include <termios.h>
#include <signal.h>

extern volatile sig_atomic_t got_sigint;
extern int shell_terminal;
extern int shell_is_interactive;
extern pid_t shell_pgid;
extern struct termios shell_tmodes;

void init_shell ();

#endif
