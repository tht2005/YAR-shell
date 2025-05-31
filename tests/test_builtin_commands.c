#include "builtin_commands/builtin_commands.h"

#include <assert.h>
#include <stdio.h>

int main ()
{
    // test pwd
    char *argv0[] = { "pwd", NULL };
    assert( exec_builtin (1, argv0) == 0 );
    // char *argv1[] = {"pwd", "--help", NULL};
    // assert( exec_builtin (2, argv1) == 0 );
    // char *argv2[] = {"pwd", NULL};
    // assert( exec_builtin (1, argv2) == 0 );

    // test cd
    // char *argv3[] = { "cd", "--help", NULL };
    // assert( exec_builtin (2, argv3) == 0 );
    // char *argv4[] = {"cd", NULL};
    // assert( exec_builtin (1, argv4) == 0 );
    // assert( exec_builtin(1, argv0) == 0);
    // char *argv5[] = {"cd", "a", "bc", NULL};
    // assert( exec_builtin (3, argv5) != 0 );
    // char *argv6[] = {"cd", "/", NULL};
    // assert( exec_builtin (1, argv6) == 0 );
    // assert( exec_builtin(1, argv0) == 0);
    // char *argv7[] = {"cd", "/", NULL};
    // exec_builtin (2, argv7);
    // assert( exec_builtin(1, argv0) == 0);
    // char *argv8[] = {"cd", "-", NULL};
    // assert( exec_builtin (2, argv8) == 0 );
    // assert( exec_builtin(1, argv0) == 0);
    return 0;
}
