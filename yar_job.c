#include "yar_job.h"
#include "yar_shell.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <termios.h>

#include "data_structure/string.h"

#ifndef WAIT_ANY
#define WAIT_ANY (-1)
#endif

job *first_job = NULL;

process *new_process ()
{
    process *p = (process *) malloc (sizeof (process));
    if (p == NULL) {
        perror ("Yar: malloc");
        exit (1);
    }
    p->next = NULL;
    p->environ = NULL;
    p->argc = 0;
    p->argv = NULL;
    p->stdin = p->stdout = p->stderr = -1;
    return p;
}
job *new_job ()
{
    job *j = (job *) malloc (sizeof (job));
    if (j == NULL) {
        perror ("Yar: malloc");
        exit (1);
    }
    j->next = NULL;
    j->command = NULL;
    j->first_process = NULL;
    j->pgid = 0;
    j->notified = 0;
    tcgetattr (shell_terminal, &j->tmodes);
    j->stdin = STDIN_FILENO;
    j->stdout = STDOUT_FILENO;
    j->stderr = STDERR_FILENO;
    j->foreground = 1;
    return j;
}
void free_process (process *p) {
    for (string *ptr = p->environ; ptr; ++ptr)
        free_string (*ptr);
    for (string *ptr = p->argv; ptr; ++ptr)
        free_string (*ptr);
    free (p->environ);
    free (p->argv);
    free (p);
}
void free_job (job *j) {
    for (process *p = j->first_process, *_next; p; p = _next) {
        _next = p->next;
        free_process (p);
    }
    if (j->command)
        free_string (j->command);
    free (j);
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
    execvp (* p->argv, p->argv);
    perror ("execvp");
    exit (1);
}

void launch_job (job *j) {

    j->next = first_job;
    first_job = j;

    // assignment case

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
            if (p->stdin == -1)
                p->stdin = infile;
            if (p->stdout == -1)
                p->stdout = outfile;
            if (p->stderr == -1)
                p->stderr = j->stderr;
            launch_process (p, j->pgid, j->foreground);
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

    // format_job_info (j, "launched");

    if (!shell_is_interactive)
        wait_for_job (j);
    else if (j->foreground)
        put_job_in_foreground (j, 0);
    else
        put_job_in_background (j, 0);

}

job * find_job (pid_t pgid) {
    job *j;
    for (j = first_job; j; j = j->next)
        if (j->pgid == pgid)
            return j;
    return NULL;
}

int job_is_stopped (job *j) {
    process *p;
    for (p = j->first_process; p; p = p->next)
        if (!p->completed && !p->stopped)
            return 0;
    return 1;
}

int job_is_completed (job *j) {
    process *p;
    for (p = j->first_process; p; p = p->next)
        if (!p->completed)
            return 0;
    return 1;
}

int mark_process_status (pid_t pid, int status) {
    job *j;
    process *p;

    if (pid > 0) {
        for (j = first_job; j; j = j->next)
            for (p = j->first_process; p; p = p->next)
                if (p->pid == pid) {
                    p->status = status;
                    if (WIFSTOPPED (status)) {
                        p->stopped = 1;
                        tcsetpgrp (shell_terminal, shell_pgid);
                        tcgetattr (shell_terminal, &j->tmodes);
                        tcsetattr (shell_terminal, TCSADRAIN, &shell_tmodes);
                    }
                    else {
                        p->completed = 1;
                        if (WIFSIGNALED (status))
                            fprintf (stderr, "%d: Terminated by signal %d.\n", (int)pid, WTERMSIG (p->status));
                    }
                    return 0;
                }
        fprintf (stderr, "No child process %d.\n", pid);
        return -1;
    }
    else if (pid == 0 || errno == ECHILD)
        return -1;
    else {
        perror ("waitpid");
        return -1;
    }
}

void update_status (void) {
    int status;
    pid_t pid;

    do
        pid = waitpid (WAIT_ANY, &status, WUNTRACED);
    while (!mark_process_status (pid, status));
}

void wait_for_job (job *j) {
    int status;
    pid_t pid;
    do
        pid = waitpid (-j->pgid, &status, WUNTRACED);
    while (!mark_process_status (pid, status)
            && !job_is_stopped (j)
            && !job_is_completed (j));
}

void format_job_info (job *j, const char *status) {
    fprintf (stderr, "%ld (%s): %s\n", (long)j->pgid, status, j->command);
}

void do_job_notification (void) {
    job *j, *jlast, *jnext;

    update_status ();

    jlast = NULL;
    for (j = first_job; j; j = jnext) {
        jnext = j->next;

        if (job_is_completed (j)) {
            format_job_info (j, "completed");
            if (jlast)
                jlast->next = jnext;
            else
                first_job = jnext;
            free_job (j);
        }
        
        else if (job_is_stopped (j) && !j->notified) {
            format_job_info (j, "stopped");
            j->notified = 1;
            jlast = j;
        }

        else
            jlast = j;
    }
}

void put_job_in_foreground (job *j, int cont) {
    tcsetpgrp (shell_terminal, j->pgid);

    if (cont) {
        tcsetattr (shell_terminal, TCSADRAIN, &j->tmodes);
        if (kill (- j->pgid, SIGCONT) < 0)
            perror ("kill (SIGCONT)");
    }

    wait_for_job (j);

    tcsetpgrp (shell_terminal, shell_pgid);
    tcgetattr (shell_terminal, &j->tmodes);
    tcsetattr (shell_terminal, TCSADRAIN, &shell_tmodes);
}

void put_job_in_background (job *j, int cont) {
    if (cont)
        if (kill (- j->pgid, SIGCONT) < 0)
            perror ("kill (SIGCONT)");
}

void mark_job_as_running (job *j) {
    process *p;
    for (p = j->first_process; p; p = p->next)
        p->stopped = 0;
    j->notified = 0;
}

void continue_job (job *j) {
    mark_job_as_running (j);
    if (j->foreground)
        put_job_in_foreground (j, 1);
    else
        put_job_in_background (j, 1);
}

