#include "yar_job.h"
#include "yar_shell.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <termios.h>

#include "data_structure/string.h"

job *first_job = NULL;

void free_process (process *p) {
    for (string *ptr = p->argv; *ptr; ++ptr)
        free_string (*ptr);
    free (p->argv);
    free (p);
}
void free_job (job *j) {
    process *p;
    for (p = j->first_process; p; p = p->next)
        free_process (p);
    free_string (j->command);
    free (j);
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
                    if (WIFSTOPPED (status))
                        p->stopped = 1;
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
        pid = waitpid (WAIT_ANY, &status, WUNTRACED);
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

void continue_job (job *j, int foreground) {
    mark_job_as_running (j);
    if (foreground)
        put_job_in_foreground (j, 1);
    else
        put_job_in_background (j, 1);
}

