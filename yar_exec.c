#include "builtin_commands/builtin_commands.h"
#include "data_structure/string.h"
#include "yar_ast.h"
#include "yar_env.h"
#include "yar_exec.h"
#include "yar_job.h"
#include "yar_shell.h"

#include <assert.h>
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
    execvp (* p->argv, p->argv);
    perror ("execvp");
    exit (1);
}

void launch_job (job *j, int foreground) {

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

void __ast_execute_command (command *command)
{
    int argc = 0;
    for (argument_list *ptr = command->arguments_and_redirections; ptr; ptr = ptr->next)
    {
        switch (ptr->type)
        {
            case AL_ARGUMENT:
                ++argc;
                break;
            case AL_REDIRECTION:
                fprintf (stderr, "Yar: not support file redirection yet! (skip)\n");
                break;
            default:
                assert (0);
                break;
        }
    }

    string *argv = (string *) malloc ((argc + 1) * sizeof (string *));
    if (argv == NULL)
    {
        perror ("Yar: malloc");
        exit (1);
    }
    argc = 0;
    for (argument_list *ptr = command->arguments_and_redirections; ptr; ptr = ptr->next)
    {
        if (ptr->type == AL_ARGUMENT)
        {
            argv[argc++] = new_string_2(ptr->arg);
        }
    }
    argv[argc] = NULL;

    if (argc == 0) {
        fprintf (stderr, "Yar: may bug: __ast_execute_command: argc = 0\n");
        for (int i = 0; i < argc; ++i) {
            free_string (argv[i]);
        }
        free (argv);
        return;
    }

    int status = exec_builtin (argc, argv);
    if (status != COMMAND_NOT_FOUND)
    {
        fprintf (stderr, "Yar: debug: execute builtin command `%s`\n", argv[0]);
        for (int i = 0; i < argc; ++i) {
            free_string (argv[i]);
        }
        free (argv);
        return;
    }

    fprintf (stderr, "Yar: debug: execute host command `%s`\n", argv[0]);

    process *p = new_process();
    p->argc = argc;
    p->argv = argv;

    job *j = new_job();
    j->first_process = p;

    launch_job (j, 1);

    free_job (j);
    // argv already be freed
}

