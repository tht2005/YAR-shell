#include "yar_env.h"
#include "yar_job.h"
#include "yar_exec.h"
#include "yar_parser.h"
#include "yar_shell.h"
#include "yar_defs.h"

#include "data_structure/string.h"

#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int source_len;
char *source;

int start;
int cur;
int inside_double_quote;
int process_spliter;

int is_process_spliter_char (char c) {
    return c == ';'
        || c == '|'
        || c == '&'
        || c == '\n';
}

int skipped_char (char c) {
    return c == '\r';
}
int is_variable_char (char c) {
    return isalpha (c)
        || isdigit (c)
        || (c == '_');
}

int end () {
    return cur >= source_len;
}

int peek_char (int step) {
    if (cur + step >= source_len) {
        return 0;
    }
    return source[cur + step];
}
int cur_char () {
    return peek_char (0);
}
int next_char () {
    for (++cur; !end () && skipped_char (cur_char()); ++cur);
    return cur_char ();
}

void skip_space () {
    while (!end () && isspace (cur_char ()))
        ++cur;
}

void init_parser (char *s) {
    source = s;
    source_len = strlen (s);
    start = 0;
    cur = 0;
    inside_double_quote = 0;
}

string next_string (string);

string next_single_quote_string (string str) {
    ++cur;
    while (!end () && cur_char () != '\'') {
        str = string_push_back (str, source[cur]);
        ++cur;
    }
    if (end ()) {
        // raise error string not end
        free_string (str);
    }
    ++cur;
    return str;
}

string next_double_quote_string (string str) {
    if (inside_double_quote != 0) {
        fprintf (stderr, "Yar: contradiction: visit next_double_quote_string but inside_db_quote != 0\n");
        exit (EXIT_FAILURE);
    }

    ++cur;
    inside_double_quote = 1;
    str = next_string (str);
    if (inside_double_quote != 0) {
        fprintf (stderr, "Yar: double quote string don't end ...\n");
        exit (EXIT_FAILURE);
    }
    return str;
}

string next_get_var (string str) {
    string var_name = new_string ();
    while (is_variable_char (cur_char())) {
        var_name = string_push_back (var_name, cur_char());
        ++cur;
    }
    char *value = getenv(var_name);
    if (value == NULL) {
        perror ("Yar: next_get_var: getenv");
        exit (EXIT_FAILURE);
    }
    str = string_append_back(str, value);
    free_string (var_name);
    return str;
}

string next_get_substitution (string str) {
    // ??????
}

int check_assignment () {
    int i = 0;
    char c;
    for (; (c = peek_char(i)) != 0; ++i) {
        if (c == '=') {
            // =    : false
            // a=   : true
            return (i > 0);
        }
        if (isdigit (c) && i == 0) {
            return 0;
        }
        if (!is_variable_char (c)) {
            return 0;
        }
        if (isspace (c)) {
            return 0;
        }
    }
    return 0;
}
// void assignment () {
//     string var_name = new_string();
//     while (cur_char() != '=') {
//         var_name = string_push_back (var_name, cur_char());
//         ++cur;
//     }
//     string value = new_string();
//     if (!end () && !isspace (cur_char())) {
//         value = next_string (value);
//     }
//     yar_setenv (var_name, value, 1);
//     free_string (var_name);
//     free_string (value);
// }

string next_string (string str) {
    char c;
    int cont_retrieve = 1;

    while (!end() && cont_retrieve) {

        switch (c = cur_char ()) {
            case '$':
                c = next_char ();
                // change get_var condition??
                if (is_variable_char (c)) {
                    str = next_get_var (str);
                }
                else if (c == '{') {
                    str = next_get_substitution (str);
                }
                else if (c == '(') {
                    c = next_char ();
                    if (c == '(') {
                        // arithmetic
                    }
                    else {
                        // sub command
                    }
                }
                break;

            case '\'':
                if (inside_double_quote) {
                    str = string_push_back (str, c);
                    ++cur;
                }
                else {
                    str = next_single_quote_string (str);
                }
                break;

            case '\"':
                if (inside_double_quote) {
                    cont_retrieve = 0;
                    inside_double_quote = 0;
                    ++cur;
                }
                else {
                    str = next_double_quote_string (str);
                }
                break;

            case '\\':
                c = next_char ();
                switch (c) {
                    case '\n':
                        ++cur;
                        break;
                    case 0:
                        // throw error
                        break;
                    default:
                        str = string_push_back (str, c);
                        ++cur;
                        printf ("here after: %d\n", cur);
                        break;
                }
                break;

            case '#':
                if (inside_double_quote) {
                    str = string_push_back (str, c);
                    ++cur;
                }
                else {
                    // comment
                }
                break;

            case ';':
            case '|':
            case '&':
            case '\n':
                if (start == cur) {
                    str = string_push_back (str, c);
                    ++cur;
                    cont_retrieve = 0;
                    break;
                }
            case ' ':
                if (inside_double_quote) {
                    str = string_push_back (str, c);
                    ++cur;
                }
                else {
                    cont_retrieve = 0;
                }
                break;

            default:
                str = string_push_back (str, c);
                ++cur;
                break;

        }
    }

    return str;
}

string get_next_string_block () {
    skip_space();
    start = cur;
    return next_string ( new_string() );
}

process * single_command () {
    int argc = 0;
    int max_argv = 4;
    string *argv = (string *) malloc (max_argv  * sizeof (string));
    if (argv == NULL) {
        fprintf (stderr, "Yar: can not allocate argv!\n");
        exit (EXIT_FAILURE);
    }

    process *p = (process *) malloc (sizeof (process));
    if (p == NULL) {
        fprintf (stderr, "Yar: can not alloc process.\n");
        exit (1);
    }
    p->next = NULL;
    p->cnt_assignment = 0;
    p->pid = 0;
    p->completed = 0;
    p->stopped = 0;
    p->status = 0;
    p->stdin = STDIN_FILENO;
    p->stdout = STDOUT_FILENO;
    p->stderr = STDERR_FILENO;
    int *file_d[] = { &(p->stdin), &(p->stdout), &(p->stderr) };

    int cont = 1;
    int assignment_pref = 1;
    while (cont) {

        if (argc >= max_argv) {
            max_argv <<= 1;
            argv = realloc (argv, max_argv * sizeof (string));
            if (argv == NULL) {
                fprintf (stderr, "Yar: can not re-allocate argv!\n");
                exit (EXIT_FAILURE);
            }
        }

        skip_space ();
        if (assignment_pref && check_assignment()) {
            ++p->cnt_assignment;
        }
        else {
            assignment_pref = 0;
        }

        skip_space ();
        
        argv[argc] = get_next_string_block ();
        
        if (cur - start == 1) {
            if (string_length (argv[argc]) != 1) {
                fprintf (stderr, "Assertion failed! argv[argc].length != 1.\n");
                exit (1);
            }

            if (is_process_spliter_char (argv[argc][0])) {
                process_spliter = argv[argc][0];
                free_string (argv[argc]);
                argv[argc] = NULL;
                break;
            }
        }

        if (string_length (argv[argc]) == 0) {
            free_string (argv[argc]);
            --argc;
            cont = 0;
        }
        else if (string_length (argv[argc]) == cur - start) {

            // redirection handling ...

            char *str = argv[argc];
            int valid_redirection = 0;

            switch (cur - start) {
                case 1:
                    if (strncmp (str, ">", 1) == 0) {
                        string outfile = get_next_string_block();
                        int fd = open (outfile, O_WRONLY | O_CREAT, WRFILE_PERM);
                        if (fd < 0) {
                            perror ("open");
                            exit (1);
                        }
                        p->stdout = fd;
                        free_string (outfile);
                        valid_redirection = 1;
                    }
                    else if (strncmp (str, "<", 1) == 0) {
                        string infile = get_next_string_block();
                        int fd = open (infile, O_RDONLY, RDFILE_PERM);
                        if (fd < 0) {
                            perror ("open");
                            exit (1);
                        }
                        p->stdin = fd;
                        free_string (infile);
                        valid_redirection = 1;
                    }

                    break;

                case 2:
                    if (strncmp (str, ">>", 2) == 0) {
                        string outfile = get_next_string_block();
                        int fd = open (outfile, O_WRONLY | O_CREAT | O_APPEND, WRFILE_PERM);
                        if (fd < 0) {
                            perror ("open");
                            exit (1);
                        }
                        p->stdout = fd;
                        free_string (outfile);
                        valid_redirection = 1;
                    }
                    else if (strncmp (str, "&>", 2) == 0) {
                        string outfile = get_next_string_block();
                        int fd = open (outfile, O_WRONLY | O_CREAT, WRFILE_PERM);
                        if (fd < 0) {
                            perror ("open");
                            exit (1);
                        }
                        p->stdout = fd;
                        p->stderr = fd;
                        free_string (outfile);
                        valid_redirection = 1;
                    }
                    break;

                case 3:
                    if (strncmp (str, "&>>", 3) == 0) {
                        string outfile = get_next_string_block();
                        int fd = open (outfile, O_WRONLY | O_CREAT | O_APPEND, WRFILE_PERM);
                        if (fd < 0) {
                            perror ("open");
                            exit (1);
                        }
                        p->stdout = fd;
                        p->stderr = fd;
                        free_string (outfile);
                        valid_redirection = 1;
                    }
                    break;

                case 4:
                    // a>&b -> redirect a equal b
                    if ('0' <= str[0] && str[0] <= '2'
                        && '0' <= str[3] && str[3] <= '2'
                        && str[1] == '>' && str[2] == '&') {

                        int a = str[0] - '0';
                        int b = str[3] - '0';

                        *(file_d[a]) = *(file_d[b]);

                        valid_redirection = 1;
                    }
                    break;

                default:
                    break;

            }

            if (valid_redirection) {
                free_string (argv[argc]);
                --argc;
            }

        }
        ++argc;
    }
    argv[argc] = NULL;
    p->argc = argc;
    p->argv = argv;
    return p;
}

job* simple_job () {
    process *last_process = NULL;

}

void test () {

    job *j = (job *) malloc (sizeof (job));
    if (j == NULL) {
        fprintf (stderr, "Yar: can not alloc job.\n");
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

    process * last_process = NULL;
    int foreground = 1;
    int empty_command = 0;

    int cont = 1;
    while (cont && !end ()) {
        process_spliter = 0;
        process *p = single_command();

        if (p->argv == NULL) {
            fprintf (stderr, "Yar: (DEV CRITICAL BUG) p->argv = (null).\n");
            exit (1);
        }

        if (p->argv[0] == NULL) {
            empty_command = 1;
            free_process (p);
            break;
        }

        if (last_process == NULL) {
            j->first_process = last_process = p;
        }
        else {
            last_process->next = p;
            last_process = p;
        }

        switch (process_spliter) {
            case '&':
                foreground = 0;
            case '\n':
            case '\0':
            case ';':
                cont = 0;
                break;

            default:
                break;
        }
    }

    if (!empty_command)
        launch_job (j, foreground);
    
    free_job (j);
}

