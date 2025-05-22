#include "environ.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define ENVIRON_INIT_SIZE       4
#define STRING_INIT_SIZE        4

typedef struct environ_t
{

    size_t size;
    size_t max_size;
    char **environ;

} environ_t;

environ_t *__extend_environ (environ_t *env)
{
    if (env->size + 3 <= env->max_size)
        return env;
    env->environ = (char **) realloc ( env->environ, (env->max_size << 1) * sizeof (char *) );
    if (env->environ == NULL) {
        perror ("__extend_environ: realloc");
    }
    else {
        env->max_size <<= 1;
    }
    return env;
}

char *__new_empty_string ()
{
    char *a = (char *) malloc (STRING_INIT_SIZE * sizeof (char));
    if (a == NULL) {
        perror ("__new_empty_string: malloc");
        return NULL;
    }
    memset (a, 0, STRING_INIT_SIZE * sizeof (char));
    return a;
}
char *__new_get_string_name (char *string)
{
    if (string == NULL) {
        fprintf (stderr, "new_get_string_name (char *string): string=NULL\n");
        return NULL;
    }

    // string=`name=value`
    // find first '=' character
    char *pos = strchr (string, '=');
    if (pos == NULL) {
        fprintf (stderr, "new_get_string_name (char *string): pos=NULL? check setenv or putenv things...\n");
        return NULL;
    }

    char *res = strndup (string, pos - string);
    if (res == NULL) {
        perror ("new_get_string_name (char *string): strndup");
        return NULL;
    }
    return res;
}
char *__new_get_string_value (char *string)
{
    if (string == NULL) {
        fprintf (stderr, "new_get_string_value (char *string): string=NULL\n");
        return NULL;
    }

    // string=`name=value`
    // find first '=' character
    char *pos = strchr (string, '=');
    if (pos == NULL) {
        fprintf (stderr, "new_get_string_value (char *string): pos=NULL? check setenv or putenv things...\n");
        return NULL;
    }

    if (pos + 1 == string + strlen (string)) {
        char *res = __new_empty_string ();
        if (res == NULL) {
            fprintf (stderr, "__new_get_string_value (char *string): res=NULL\n");
        }
        return res;
    }

    char *res = strndup (pos + 1, (string - pos) + strlen (string) - 1);
    if (res == NULL) {
        perror ("new_get_string_value (char *string): strndup");
    }
    return res;
}

int __valid_name_2 (char *begin, char *end, int strict)
{
    // name can't be null/empty
    if (begin >= end)
    {
        return -1;
    }

    for (; begin != end; ++begin)
    {
        if (!isalnum (*begin) && (*begin) != '_') {
            return -2;
        }
    }

    if (strict)
    {
        if (isdigit (*begin)) {
            return -69;
        }
    }

    return 0;
}
int __valid_name (char *begin, int strict)
{
    return __valid_name_2 (begin, begin + strlen(begin), strict);
}
int __valid_string (char *string, int strict)
{
    char *ptr = strchr (string, '=');
    if (ptr == NULL)
        return -1;
    if (__valid_name_2 (string, ptr, strict))
        return -2;
    return 0;
}

int __check_same_name (char *string0, char *string1)
{
    char *p0 = strchr (string0, '=');
    char *p1 = strchr (string1, '=');
    if (p0 == NULL || p1 == NULL) {
        fprintf (stderr, "__check_same_name: p0=NULL or p1=NULL\n");
        return -1;
    }
    size_t len0 = p0 - string0;
    size_t len1 = p1 - string1;
    if (len0 != len1)
        return -2;
    return strncmp (string0, string1, len0) ? -3 : 0;
}

char *__build_string (char *name, char *value)
{
    size_t a = strlen (name);
    size_t b = strlen (value);
    char *res = (char *) malloc ((a + b + 3) * sizeof (char));
    if (res == NULL) {
        perror ("__build_string (char *name, char *value): malloc");
        return NULL;
    }
    memset (res, 0, (a + b + 3) * sizeof (char));
    memcpy (res, name, a * sizeof (char));
    res[a] = '=';
    memcpy (res + a + 1, value, b * sizeof (char));
    return res;
}

// void __swap_string (char **a, char **b)
// {
//     char *c = *a;
//     *a = *b;
//     *b = c;
// }

size_t environ_size (environ_t *env) {
    return env->size;
}

environ_t *new_environ ()
{
    environ_t *res = (environ_t *) malloc (sizeof (environ_t));
    if (res == NULL) {
        perror ("new_environ: malloc");
        return NULL;
    }

    res->size = 0;
    res->max_size = ENVIRON_INIT_SIZE;
    res->environ = (char **) malloc (ENVIRON_INIT_SIZE * sizeof (char *));
    if (res->environ == NULL) {
        perror ("new_environ: malloc");
        return NULL;
    }
    memset (res->environ, 0, ENVIRON_INIT_SIZE * sizeof (char *));
    return res;
}
void free_environ (environ_t *env)
{
    for (char **p = env->environ; *p; ++p)
        free (*p);
    free (env->environ);
    free (env);
}

char* getenv_nofree (environ_t *env, char *name)
{
    for (char **p = env->environ; *p; ++p) {
        char *val = __new_get_string_name (*p);
        if (val == NULL) {
            fprintf (stderr, "getenv_ (char **environ, char *name): val=NULL\n");
            return NULL;
        }
        if (strcmp (name, val) == 0) {
            free (name);
            free (val);
            char *res = __new_get_string_value (*p);
            if (res == NULL) {
                fprintf (stderr, "getenv_ (char **environ, char *name): res=NULL\n");
            }
            return res;
        }
        free (val);
    }
    free (name);
    return NULL;
}

int putenv_nofree (environ_t *env, char *string)
{
    // check if string is valid
    if (__valid_string (string, 1)) {
        free (string);
        return -1;
    }

    // check there is a string with same name
    for (char **p = env->environ; *p; ++p) {
        if (__check_same_name (string, *p) == 0) {
            free (*p);
            *p = string;
            return 0;
        }
    }

    // add new string
    __extend_environ (env);
    char **p = env->environ + env->size;
    *p = string;
    ++env->size;
    return 0;
}

int setenv_nofree (environ_t *env, char *name, char *value, int replace)
{
    // check if name is valid
    if (__valid_name (name, 1)) {
        free(name);
        free(value);

        return -1;
    }

    // check there is a string with the same name
    for (char **p = env->environ; *p; ++p) {
        char *val = __new_get_string_name (*p);
        if (val == NULL) {
            fprintf (stderr, "setenv_ (char **environ, char *name, char *value, char *replace): val=NULL\n");
            return -2;
        }
        if (strcmp (val, name) == 0) {
            if (replace) {
                free (*p);
                char *string = __build_string (name, value);
                if (string == NULL) {
                    perror ("setenv_: __build_string");
                }
                *p = string;
            }
            free (name);
            free (value);
            return 0;
        }
    }

    // add new string
    __extend_environ (env);
    char **p = env->environ + env->size;
    char *string = __build_string (name, value);
    if (string == NULL) {
        perror ("setenv_: __build_string");
    }
    *p = string;
    ++env->size;
    free (name);
    free (value);
    return 0;
}

int unsetenv_nofree (environ_t *env, char *name)
{
    // check if name is valid
    if (__valid_name (name, 1)) {
        free(name);
        return -1;
    }
    for (char **p = env->environ; *p; ++p) {
        char *val = __new_get_string_name (*p);
        if (val == NULL) {
            free (name);
            fprintf (stderr, "unsetenv_ (char **environ, char *name): val=NULL\n");
            return -2;
        }
        if (strcmp (val, name) == 0) {
            free (*p);
            free (name);
            --env->size;
            char **q = env->environ + env->size;
            *p = *q;
            *q = NULL;
            return 0;
        }
    }
    free (name);
    return 0;
}

