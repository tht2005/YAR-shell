#include "yar_shell.h"
#include "yar_job.h"

#include <errno.h>
#include <stdio.h>
#include <readline/readline.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

volatile sig_atomic_t got_sigint = 0;

pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int shell_is_interactive;

void sigint_handler (int signo)
{
    got_sigint = 1;
    // rl_replace_line ("", 0);
    // rl_done = 1;
    // write (STDOUT_FILENO, "\n", 1);
    // rl_cleanup_after_signal();
    rl_replace_line ("", 0);
    rl_on_new_line();
    write (STDOUT_FILENO, "\n", 1);
    rl_done = 1;
    tcflush (shell_terminal, TCIFLUSH);
    rl_forced_update_display();
}

void sigchld_handler (int signo)
{
    int saved_error = errno;
    pid_t pid;
    int status;

    while ((pid = waitpid (WAIT_ANY, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
        mark_process_status (pid, status);
    }
}

void init_shell () {
    shell_terminal = STDIN_FILENO;
    shell_is_interactive = isatty (shell_terminal);
    if (shell_is_interactive) {
        while (tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp ())) {
            kill (- shell_pgid, SIGTTIN);
        }

        signal (SIGINT, sigint_handler);
        // signal (SIGINT, SIG_IGN);
        signal (SIGQUIT, SIG_IGN);
        signal (SIGTSTP, SIG_IGN);
        signal (SIGTTIN, SIG_IGN);
        signal (SIGTTOU, SIG_IGN);
        signal (SIGCHLD, sigchld_handler);

        shell_pgid = getpid ();
        if (setpgid (shell_pgid, shell_pgid) < 0) {
            perror ("Couldn't put the shell in its own process group");
            exit (1);
        }

        tcsetpgrp (shell_terminal, shell_pgid);
        tcgetattr (shell_terminal, &shell_tmodes);
    }
}
