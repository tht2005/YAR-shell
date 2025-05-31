#include "builtin_commands.h"
#include <argp.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../data_structure/string.h"


void cd_pre ()
{
}

void cd_post ()
{
    // giai phong bo nho?
}

int cd_main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        printf("Usage: cd [DIRECTORY]\n");
        printf("Change the current working directory.\n\n");
        printf("Arguments:\n");
        printf("  DIRECTORY      The path to change into. Can be absolute or relative.\n\n");
        printf("Common Usages:\n");
        printf("  cd             Go to the home directory.\n");
        printf("  cd /           Go to the root directory.\n");
        printf("  cd ..          Move up one directory level.\n");
        printf("  cd -           Go back to the previous directory.\n");
        printf("  cd ~/folder    Go to a folder in your home directory.\n\n");
        fflush(stdout);
        return 0;
    }

    char *dir, *oldpwd = getenv("PWD");

    if (argc > 2) {
        printf("cd: too many arguments\n");
        return 1;
    }

    if (argc == 1) {
        dir = getenv("HOME");
        if (dir == NULL) {
            fprintf(stderr, "cd: HOME environment variable not set\n");
            return 1;
        }
    }
    else {
        if (strcmp(argv[1], "-") == 0) {
            dir = getenv("OLDPWD");
            if (dir == NULL) {
                printf("cd: OLDPWD environment variable not set\n");
                return 1;
            }
        }
        else {
            string des = new_string();
            int idx = 0;
            if (argv[1][0] == '~') {
                char *home = getenv("HOME");
                if (home == NULL) {
                    printf("cd: ROOT environment variable not set\n");
                    return 1;
                }
                des = string_append_back(des, home);
                idx = 1;
            }
            else {
                if (argv[1][0] == '/') {
                    des = string_push_back(des, '/');
                    idx = 1;
                }
                else {
                    des = string_append_back(des, oldpwd);
                }
            }

            des = string_push_back(des, '/');
            des = string_append_back(des, argv[1] + idx);
            dir = realpath(des, NULL);
            if (dir == NULL) {
                printf("cd: %s: No such file or directory\n", argv[1]);
                free_string(des);
                free(dir);
                return 1;
            }

            setenv("OLDPWD", oldpwd, 1);
            setenv("PWD", dir, 1);

            free(dir);
            free_string(des);
            return 0;
        }
    }

    setenv("OLDPWD", oldpwd, 1);
    setenv("PWD", dir, 1);

    fflush (stdout);
    return 0;
}

__attribute__((constructor))
static void __cd_register (void)
{
    builtin_command_register("cd", cd_main, cd_pre, cd_post);
}

