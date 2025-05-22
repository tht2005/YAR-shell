#include "data_structure/environ.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

char *new_string (const char *str) {
    return strdup (str);
}

void _assert (environ_t *env, char *name, char *value)
{
    char *res = getenv_nofree (env, new_string(name));
    assert (strcmp (res, value) == 0);
    free (res);
}

int main ()
{

    environ_t *env = new_environ ();
    assert (environ_size (env) == 0);
    putenv_nofree (env, new_string ("abc=def"));
    assert (environ_size (env) == 1);
    putenv_nofree (env, new_string ("PATH=/usr/bin/abc:/usr/bin/zzz"));
    assert (environ_size (env) == 2);
    putenv_nofree (env, new_string ("equal=equal=equal"));
    assert (environ_size (env) == 3);
    putenv_nofree (env, new_string ("null="));
    assert (environ_size (env) == 4);

    // test getenv_
    _assert (env, "abc", "def");
    _assert (env, "PATH", "/usr/bin/abc:/usr/bin/zzz"); 
    _assert (env, "equal", "equal=equal");
    // edge case
    _assert (env, "null", "");
    // // not available
    assert ( getenv_nofree (env, new_string ("varvarvar")) == NULL );
    assert ( getenv_nofree (env, new_string ("xyz123")) == NULL );


    // test putenv_
    putenv_nofree (env, new_string ("null=abc"));
    _assert (env, "null", "abc");
    assert (environ_size (env) == 4);
    putenv_nofree (env, new_string ("va12=zk $_ { \\"));
    _assert (env, "va12", "zk $_ { \\");
    assert (environ_size (env) == 5);

    // test setenv_
    setenv_nofree (env, new_string ("zzz"), new_string("gg"), 1);
    _assert (env, "zzz", "gg");
    assert (environ_size (env) == 6);
    setenv_nofree (env, new_string ("zzz"), new_string("ksdjflkdfjsl"), 0);
    _assert (env, "zzz", "gg");
    assert (environ_size (env) == 6);
    setenv_nofree (env, new_string ("zzz"), new_string("sxsx"), 1);
    _assert (env, "zzz", "sxsx");
    assert (environ_size (env) == 6);

    // test unsetenv_
    unsetenv_nofree (env, new_string ("fdjsfkdsl"));
    assert (environ_size (env) == 6);
    unsetenv_nofree (env, new_string ("PATH"));
    assert (environ_size (env) == 5);
    _assert (env, "abc", "def");
    _assert (env, "equal", "equal=equal");
    _assert (env, "null", "abc");
    _assert (env, "va12", "zk $_ { \\");
    _assert (env, "zzz", "sxsx");
    assert (getenv_nofree (env, new_string ("PATH")) == NULL);

    free_environ (env);

    return 0;
}

