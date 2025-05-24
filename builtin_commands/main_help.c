#include "builtin_commands.h"

#include <stdio.h>

int main_help(int argc, char **argv)
{
    puts(
"Yar: Simple scripting language."

"Builtin Commands:"
"\thelp: Show help"
"\tls: show current directory's content"
"\tpwd: show current directory"
);
    fflush (stdout);
    return 0;
}

