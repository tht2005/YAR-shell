#include <dirent.h>
#include <errno.h>

#include "builtin_commands.h"

#include <stdio.h>
#include <string.h>
#include "../data_structure/string.h"

void ls_pre ()
{
}

void ls_post ()
{
}

#include <ctype.h>

int is_any_whitespace(const char *str) {
    if (str == NULL) return 0;

    while (*str) {
        if (isspace((unsigned char)*str)) {
            return 1;
        }
        str++;
    }
    return 0;
}


int ls_main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        printf("Usage: ls [DIRECTORY]...\n\n");
        printf("List information about the contents of one or more directories.\n");
        printf("If no DIRECTORY is specified, the current directory is used.\n\n");

        fflush(stdout);
        return 0;
    }
    char *dir, *oldpwd = getenv("PWD");
    string can = new_string(), canfile = new_string();
    for (int i = 1; i < argc; i++) {
        string des = new_string();
        int idx = 0;
        if (argv[i][0] == '~') {
            char *home = getenv("HOME");
            if (home == NULL) {
                printf("ls: cannot access %s: Home environment variable not set\n", argv[i]);
                free_string(des);
                continue;
            }
            des = string_append_back(des, home);
            idx = 1;
        }
        else {
            if (argv[i][0] == '/') {
                des = string_push_back(des, '/');
                idx = 1;
            }
            else {
                des = string_append_back(des, oldpwd);
            }
        }

        des = string_push_back(des, '/');
        des = string_append_back(des, argv[i] + idx);
        dir = realpath(des, NULL);
        if (dir == NULL) {
            printf("ls: cannot access '%s': No such file or directory\n", argv[i]);
            free_string(des);
            free(dir);
            continue;
        }

        DIR *cdir = opendir(dir);
        if (cdir == NULL) {
            if (strcmp(strerror(errno), "Not a directory") == 0) {
                if (is_any_whitespace(argv[i])) {
                    canfile = string_append_back(canfile, "'");
                    canfile = string_append_back(canfile, argv[i]);
                    canfile = string_append_back(canfile, "'");
                }
                else {
                    canfile = string_append_back(canfile, argv[i]);
                }
                canfile = string_append_back(canfile, "\n");
            }
            else {
                printf("ls: cannot access '%s': %s\n", argv[i], strerror(errno));
            }
            free(dir);
            free_string(des);
            continue;
        }

        can = string_append_back(can, dir);
        can = string_append_back(can, ":\n");

        struct dirent *entry;
        while ((entry = readdir(cdir)) != NULL) {
            if (entry->d_name[0] == '.') continue;

            if (is_any_whitespace(entry->d_name)) {
                can = string_append_back(can, "'");
                can = string_append_back(can, entry->d_name);
                can = string_append_back(can, "'");
            }
            else {
                can = string_append_back(can, entry->d_name);
            }
            can = string_append_back(can, "\n");
        }
        can = string_append_back(can, "\n");

        free(dir);
        free_string(des);
    }

    if (argc == 1) {
        dir = getenv("PWD");

        DIR *cdir = opendir(dir);
        if (cdir == NULL) {
            printf("ls: cannot access '%s': %s\n\n", dir, strerror(errno));
        }
        else {
            struct dirent *entry;
            while ((entry = readdir(cdir)) != NULL) {
                if (entry->d_name[0] == '.') continue;
                if (is_any_whitespace(entry->d_name)) {
                    can = string_append_back(can, "'");
                    can = string_append_back(can, entry->d_name);
                    can = string_append_back(can, "'");
                }
                else {
                    can = string_append_back(can, entry->d_name);
                }
                can = string_append_back(can, "\n");
            }
            can = string_append_back(can, "\n\n");
        }
    }

    printf("%s\n%s", canfile, can);
    free_string(can);
    free_string(canfile);
    fflush (stdout);
    return 0;
}

__attribute__((constructor))
static void __ls_register (void)
{
    builtin_command_register("ls", ls_main, ls_pre, ls_post);
}

