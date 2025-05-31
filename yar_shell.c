#include "yar_shell.h"

#include <sys/types.h>
#include <termios.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int shell_is_interactive;

void sigint_handler (int sig)
{
    exit (0);
}

void init_shell () {
    shell_terminal = STDIN_FILENO;
    shell_is_interactive = isatty (shell_terminal);
    if (shell_is_interactive) {
        while (tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp ())) {
            kill (- shell_pgid, SIGTTIN);
        }

        signal (SIGINT, sigint_handler);
        signal (SIGQUIT, SIG_IGN);
        signal (SIGTSTP, SIG_IGN);
        signal (SIGTTIN, SIG_IGN);
        signal (SIGTTOU, SIG_IGN);
        signal (SIGCHLD, SIG_IGN);

        shell_pgid = getpid ();
        if (setpgid (shell_pgid, shell_pgid) < 0) {
            perror ("Couldn't put the shell in its own process group");
            exit (1);
        }

        tcsetpgrp (shell_terminal, shell_pgid);

        tcgetattr (shell_terminal, &shell_tmodes);
    }
}
