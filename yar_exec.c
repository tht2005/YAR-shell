#include "builtin_commands/builtin_commands.h"
#include "data_structure/string.h"
#include "yar_ast.h"
#include "yar_exec.h"
#include "yar_job.h"
#include "yar_shell.h"
#include "yar_parser.tab.h"
#include "yar_defs.h"
#include "yar_interpreter.h"

#include <assert.h>
#include <fcntl.h>
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

int __open_wrapper (const char *file, int oflag, int perm)
{
    int fd = open (file, oflag, perm);
    if (fd < 0) {
        perror ("open");
        runtime_error = 1;
    }
    return fd;
}
void __do_redirect (int type, string file, int *stdin, int *stdout, int *stderr)
{
    switch (type)
    {
        case LESS:
            *stdin = __open_wrapper(file, O_RDONLY, RDFILE_PERM); 
            break;
        case GREATER:
            *stdout = __open_wrapper (file, O_WRONLY | O_CREAT | O_TRUNC, WRFILE_PERM);
            break;
        case GREATER_DOUBLE:
            *stdout = __open_wrapper (file, O_WRONLY | O_CREAT | O_APPEND, WRFILE_PERM);
            break;
        case AND_GREATER:
        case GREATER_AND:
            *stdout = *stderr = __open_wrapper (file, O_WRONLY | O_CREAT | O_TRUNC, WRFILE_PERM);
            break;
        case AND_GREATER_DOUBLE:
            *stdout = *stderr = __open_wrapper (file, O_WRONLY | O_CREAT | O_APPEND, WRFILE_PERM);
            break;

        case NUM_LESS:
        case NUM_GREATER:
        case NUM_LESS_AND:
        case NUM_GREATER_AND:
        default:
            assert (0);
            break;
    }
}

int __dup_wrapper (int fd_)
{
    int fd = dup(fd_);
    if (fd < 0) {
        perror ("fd");
        runtime_error = 1;
    }
    return fd;
}
int __dup2_wrapper (int oldfd, int newfd)
{
    if (dup2 (oldfd, newfd) < 0) {
        perror ("dup2");
        runtime_error = 1;
    }
    return 0;
}

process *__ast_build_process (command *command)
{
    process *p = new_process();

    int __stdin = STDIN_FILENO, __stdout = STDOUT_FILENO, __stderr = STDERR_FILENO;
    int argc = 0;
    for (argument_list *ptr = command->arguments_and_redirections; ptr; ptr = ptr->next)
    {
        switch (ptr->type)
        {
            case AL_ARGUMENT:
                ++argc;
                break;
            case AL_REDIRECTION:
                __do_redirect (ptr->redirection.type, ptr->redirection.file, &__stdin, &__stdout, &__stderr);
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
        if (__stdin != STDIN_FILENO) close (__stdin);
        if (__stdout != STDOUT_FILENO) close (__stdout);
        if (__stderr != STDERR_FILENO) close (__stderr);
        exit (1);
    }
    argc = 0;
    for (argument_list *ptr = command->arguments_and_redirections; ptr; ptr = ptr->next)
    {
        if (ptr->type == AL_ARGUMENT)
        {
            argv[argc++] = new_string_2 (ptr->arg);
        }
    }
    argv[argc] = NULL;

    if (argc == 0) {
        fprintf (stderr, "Yar: may bug: __ast_build_command: argc = 0\n");
        for (int i = 0; i < argc; ++i) {
            free_string (argv[i]);
        }
        free (argv);
        if (__stdin != STDIN_FILENO) close (__stdin);
        if (__stdout != STDOUT_FILENO) close (__stdout);
        if (__stderr != STDERR_FILENO) close (__stderr);
        return NULL;
    }

    size_t count = string_list_count (command->assignments);
    string *environ = (string *) malloc ((count + 1) * sizeof (string));
    if (environ == NULL) {
        perror ("Yar: malloc");
        exit (1);
    }
    count = 0;
    for (string_list *ptr = command->assignments; ptr; ptr = ptr->next)
        environ[count++] = new_string_2 (ptr->str);
    environ[count] = NULL;
    p->environ = environ;
    p->argc = argc;
    p->argv = argv;
    if (__stdin != STDIN_FILENO)
        p->stdin = __stdin;
    if (__stdout != STDOUT_FILENO)
        p->stdout = __stdout;
    if (__stderr != STDERR_FILENO)
        p->stderr = __stderr;
    return p;
}

// fix this as an built-in command exec
void __ast_execute_command (command *command)
{
    int __stdin = STDIN_FILENO, __stdout = STDOUT_FILENO, __stderr = STDERR_FILENO;
    int argc = 0;
    for (argument_list *ptr = command->arguments_and_redirections; ptr; ptr = ptr->next)
    {
        switch (ptr->type)
        {
            case AL_ARGUMENT:
                ++argc;
                break;
            case AL_REDIRECTION:
                __do_redirect (ptr->redirection.type, ptr->redirection.file, &__stdin, &__stdout, &__stderr);
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

    {
        int saved_stdin = __dup_wrapper (STDIN_FILENO);
        int saved_stdout = __dup_wrapper (STDOUT_FILENO);
        int saved_stderr = __dup_wrapper (STDERR_FILENO);
        if (__stdin != STDIN_FILENO && __stdin >= 0)
        {
            __dup2_wrapper (__stdin, STDIN_FILENO);
        }
        if (__stdout != STDOUT_FILENO && __stdout >= 0)
        {
            __dup2_wrapper (__stdout, STDOUT_FILENO);
        }
        if (__stderr != STDERR_FILENO && __stderr >= 0)
        {
            __dup2_wrapper (__stderr, STDERR_FILENO);
        }

        int status = exec_builtin (argc, argv);

        __dup2_wrapper (saved_stdin, STDIN_FILENO);
        __dup2_wrapper (saved_stdout, STDOUT_FILENO);
        __dup2_wrapper (saved_stderr, STDERR_FILENO);

        close (saved_stdin);
        close (saved_stdout);
        close (saved_stderr);

        if (__stdin != STDIN_FILENO) close (__stdin);
        if (__stdout != STDOUT_FILENO) close (__stdout);
        if (__stderr != STDERR_FILENO) close (__stderr);

        if (status != COMMAND_NOT_FOUND)
        {
            fprintf (stderr, "Yar: debug: execute builtin command `%s`\n", argv[0]);
            for (int i = 0; i < argc; ++i) {
                free_string (argv[i]);
            }
            free (argv);
            return;
        }
    }

    fprintf (stderr, "Yar: debug: execute host command `%s`\n", argv[0]);

    process *p = new_process();
    p->argc = argc;
    p->argv = argv;

    job *j = new_job();
    j->first_process = p;
    j->stdin = __stdin;
    j->stdout = __stdout;
    j->stderr = __stderr;

    // launch_job (j, 1);

    free_job (j);
    // argv already be freed
}

