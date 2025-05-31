#include "data_structure/environ.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

char *newstr (const char *str) {
    return strdup (str);
}

void _assert (environ_t *env, char *name, char *value)
{
    char *res = getenv_nofree (env, newstr(name));
    assert (strcmp (res, value) == 0);
    free (res);
}

int main ()
{

    environ_t *env = new_environ ();
    assert (environ_size (env) == 0);
    putenv_nofree (env, newstr ("abc=def"));
    assert (environ_size (env) == 1);
    putenv_nofree (env, newstr ("PATH=/usr/bin/abc:/usr/bin/zzz"));
    assert (environ_size (env) == 2);
    putenv_nofree (env, newstr ("equal=equal=equal"));
    assert (environ_size (env) == 3);
    putenv_nofree (env, newstr ("null="));
    assert (environ_size (env) == 4);

    // test getenv_
    _assert (env, "abc", "def");
    _assert (env, "PATH", "/usr/bin/abc:/usr/bin/zzz"); 
    _assert (env, "equal", "equal=equal");
    // edge case
    _assert (env, "null", "");
    // // not available
    assert ( getenv_nofree (env, newstr ("varvarvar")) == NULL );
    assert ( getenv_nofree (env, newstr ("xyz123")) == NULL );


    // test putenv_
    putenv_nofree (env, newstr ("null=abc"));
    _assert (env, "null", "abc");
    assert (environ_size (env) == 4);
    putenv_nofree (env, newstr ("va12=zk $_ { \\"));
    _assert (env, "va12", "zk $_ { \\");
    assert (environ_size (env) == 5);

    // test setenv_
    setenv_nofree (env, newstr ("zzz"), newstr("gg"), 1);
    _assert (env, "zzz", "gg");
    assert (environ_size (env) == 6);
    setenv_nofree (env, newstr ("zzz"), newstr("ksdjflkdfjsl"), 0);
    _assert (env, "zzz", "gg");
    assert (environ_size (env) == 6);
    setenv_nofree (env, newstr ("zzz"), newstr("sxsx"), 1);
    _assert (env, "zzz", "sxsx");
    assert (environ_size (env) == 6);

    // test unsetenv_
    unsetenv_nofree (env, newstr ("fdjsfkdsl"));
    assert (environ_size (env) == 6);
    unsetenv_nofree (env, newstr ("PATH"));
    assert (environ_size (env) == 5);
    _assert (env, "abc", "def");
    _assert (env, "equal", "equal=equal");
    _assert (env, "null", "abc");
    _assert (env, "va12", "zk $_ { \\");
    _assert (env, "zzz", "sxsx");
    assert (getenv_nofree (env, newstr ("PATH")) == NULL);

    free_environ (env);

    return 0;
}

