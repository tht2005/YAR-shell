#include "builtin_commands/builtin_commands.h"

#include <assert.h>
#include <stdio.h>

int main ()
{
    char *argv[] = { "help", NULL };
    assert( exec_builtin (1, argv) == 0 );
    return 0;
}
