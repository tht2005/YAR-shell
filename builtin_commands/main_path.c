#include "builtin_commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../data_structure/string.h"

void path_pre ()
{
    // khoi tao?
}

void path_post ()
{
    // giai phong bo nho?
}

int path_main(int argc, char **argv)
{
    if (argc == 1) {
        char *copy = strdup (getenv("PATH")), *saveptr;
        char *token = strtok_r (copy, ":", &saveptr);
        printf ("Environment paths:\n");
        int count = 0;
        while (token != NULL)
        {
            printf ("    [%d] %s\n", ++count, token);
            token = strtok_r (NULL, ":", &saveptr);
        }
        free (copy);
        return 0;
    }

    if (argc == 3) {
        if (strcmp (argv[1], "-a") == 0) {
            if (strlen (argv[2]) == 0) {
                fprintf (stderr, "Path can not be empty!\n");
                return -1;
            }
            string copy = new_string_2 (getenv ("PATH"));
            copy = string_push_back (copy, ':');
            copy = string_append_back (copy, argv[2]);
            setenv ("PATH", copy, 1);
            free_string (copy);
            return 0;
        }

        if (strcmp (argv[1], "-d") == 0) {
            int index;
            if (safe_str_to_int (argv[2], &index)) {
                fprintf (stderr, "Invalid index.\n");
                return -2;
            }
            string value = new_string();
            char *copy = strdup (getenv("PATH")), *saveptr;
            char *token = strtok_r (copy, ":", &saveptr);
            int count = 0;
            while (token != NULL)
            {
                ++count;
                if (count != index) {
                    if (string_length (value))
                        value = string_push_back (value, ':');
                    value = string_append_back (value, token);
                }
                token = strtok_r (NULL, ":", &saveptr);
            }
            setenv ("PATH", value, 1);
            free (copy);
            free_string (value);
            return 0;
        }
    }

    puts ("Usage: path");
    puts ("       path -a [dir]");
    puts ("       path -d [index]");
    return -5;
}

__attribute__((constructor))
static void __path_register (void)
{
    builtin_command_register("path", path_main, path_pre, path_post);
}

