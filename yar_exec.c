#include "yar_env.h"
#include "yar_exec.h"
#include "yar_job.h"
#include "yar_shell.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

size_t __cnt_processes (job *j)
{
    size_t res = 0;
    for (process *p = j->first_process; p; p = p->next)
        ++res;
    return res;
}

void launch_process (process *p, pid_t pgid, int foreground) {
    pid_t pid;

    if (shell_is_interactive) {
        pid = getpid ();
        if (pgid == 0) pgid = pid;
        setpgid (pid, pgid);
        if (foreground)
            tcsetpgrp (shell_terminal, pgid);

        signal (SIGINT, SIG_DFL);
        signal (SIGQUIT, SIG_DFL);
        signal (SIGTSTP, SIG_DFL);
        signal (SIGTTIN, SIG_DFL);
        signal (SIGTTOU, SIG_DFL);
        signal (SIGCHLD, SIG_DFL);
    }

    if (p->stdin != STDIN_FILENO) {
        dup2 (p->stdin, STDIN_FILENO);
        close (p->stdin);
    }

    if (p->stdout != STDOUT_FILENO) {
        dup2 (p->stdout, STDOUT_FILENO);
        close (p->stdout);
    }

    if (p->stderr != STDERR_FILENO) {
        dup2 (p->stderr, STDERR_FILENO);
        close (p->stderr);
    }

    // init environment
    int i;
    for (i = 0; i < (p->cnt_assignment); ++i) {
        if (putenv ( strdup(p->argv[i])) ) {
            perror ("putenv");
            exit (1);
        } 
    }
    
    execvp (p->argv[i], &(p->argv[i]));
    perror ("execvp");
    exit (1);
}

void launch_job (job *j, int foreground) {

    // assignment case
    if (__cnt_processes (j) == 1
            && j->first_process->cnt_assignment == j->first_process->argc) {

        fprintf (stderr, "DEBUG: you just assign variables!\n");

        process *p = j->first_process;
        for (int i = 0; i < (p->cnt_assignment); ++i) {
            if (putenv (p->argv[i])) {
                perror ("putenv");
                exit (1);
            } 
        }
        return;
    }

    process *p;
    pid_t pid;
    int mypipe[2], infile, outfile;

    infile = j->stdin;
    for (p = j->first_process; p; p = p->next) {
        if (p->next) {
            if (pipe (mypipe) < 0) {
                perror ("pipe");
                exit (1);
            }
            outfile = mypipe[1];
        }
        else {
            outfile = j->stdout;
        }

        pid = fork ();

        if (pid == 0) {
            if (p->stdin == STDIN_FILENO)
                p->stdin = infile;
            if (p->stdout == STDOUT_FILENO)
                p->stdout = outfile;
            if (p->stderr == STDERR_FILENO)
                p->stderr = j->stderr;
            launch_process (p, j->pgid, foreground);
        }
        else if (pid < 0) {
            perror ("fork");
            exit (1);
        }
        else {
            p->pid = pid;
            if (shell_is_interactive) {
                if (!j->pgid)
                    j->pgid = pid;
                setpgid (pid, j->pgid);
            }
        }

        if (infile != j->stdin)
            close (infile);
        if (outfile != j->stdout)
            close (outfile);

        infile = mypipe[0];
    }

    format_job_info (j, "launched");

    if (!shell_is_interactive)
        wait_for_job (j);
    else if (foreground)
        put_job_in_foreground (j, 0);
    else
        put_job_in_background (j, 0);

}

